//
// Created by finns on 21.02.26.
//

#include "x86_64elf.h"

#include <cstring>
#include <iostream>
#include <memory>
#include <ranges>
#include <vector>
#include <string>
#include <format>

#include "cppRustDemangler.h"

x86_64elf::x86_64elf(const std::string &path) : ElfHandler(path) {
    currentFile.open(path);
    demangler = std::make_unique<cppRustDemangler>();

    if (!currentFile.is_open()) {
        handeFileError("Could not open file: " + path);
    }

    std::vector<char> buffer(16);
    currentFile.read(buffer.data(), static_cast<long>(buffer.size()));

    if (!currentFile.good()) {
        handeFileError("Could not read e_ident");
    }

    //Magic Number
    if (std::memcmp(buffer.data(),ELFMAG, SELFMAG) != 0) {
        handeFileError("Wrong Magic Number. Not an Elf File");
    }

    //64 bit
    if (buffer[EI_CLASS] != ELFCLASS64) {
        handeFileError("Wrong Class. 64bit");
    }

    //endiannes
    if (buffer[EI_DATA] != ELFDATA2LSB) {
        handeFileError("Endian not supported");
    }

    //version
    if (buffer[EI_VERSION] != EV_CURRENT) {
        handeFileError("Wrong Version");
    }

    //ABI
    if (buffer[EI_OSABI] != ELFOSABI_NONE && buffer[EI_OSABI] != ELFOSABI_GNU && buffer[EI_OSABI] != ELFOSABI_LINUX) {
        handeFileError("Not a Linux ABI");
    }


    eHdr = std::make_unique<Elf64_Ehdr>();

    currentFile.seekg(0, std::ios::beg);
    currentFile.read(reinterpret_cast<std::istream::char_type *>(eHdr.get()), sizeof(Elf64_Ehdr));

    if (!currentFile.good()) {
        handeFileError("Could not read ElfHeader");
    }

    // Executable or shared Object
    if (eHdr->e_type != ET_EXEC && eHdr->e_type != ET_DYN) {
        handeFileError("Not an executable");
    }

    if (eHdr->e_machine != EM_X86_64) {
        handeFileError("Wrong ISA");
    }

    //load section headers
    sectionHeaders.resize(eHdr->e_shnum);
    currentFile.seekg(static_cast<long>(eHdr->e_shoff), std::ios::beg);
    currentFile.read(reinterpret_cast<std::istream::char_type *>(sectionHeaders.data()),
                     static_cast<long>(eHdr->e_shnum * sizeof(Elf64_Shdr)));

    if (!currentFile.good()) {
        handeFileError("Could not read section headers");
    }

    //load program headers
    programHeaders.resize(eHdr->e_phnum);
    currentFile.seekg(static_cast<long>(eHdr->e_phoff), std::ios::beg);
    currentFile.read(reinterpret_cast<std::istream::char_type *>(programHeaders.data()),
                     static_cast<long>(eHdr->e_phnum * sizeof(Elf64_Phdr)));

    if (!currentFile.good()) {
        handeFileError("Could not read program headers");
    }

    //stringTable
    createStringTables();
    stringTable = stringTables[eHdr->e_shstrndx];
    createSymbolTables();

    for (auto &[key,value]: symbolAddressTable) {
        //std::cout << "address 0x" << std::hex << key << ": " << value << std::endl;
    }

    for (const auto &segment: sectionHeaders) {
        segmentsIndex[&stringTable[segment.sh_name]] = segment;
    }

    for (const auto &[key, value]: relaTables) {
        std::cout << "Relocations for " << &stringTable[sectionHeaders[key].sh_name] << std::endl;
        for (const auto &rela: value) {
            const auto syms = symbolTables[sectionHeaders[key].sh_link][ELF64_R_SYM(rela.r_info)];
            uint32_t strtabIndex = sectionHeaders[sectionHeaders[key].sh_link].sh_link;
            std::cout << &stringTables[strtabIndex][syms.st_name] << std::endl;
        }
        std::cout << "end " << std::endl;
    }
}

void x86_64elf::handeFileError(const std::string &errMsg) {
    if (currentFile.is_open()) {
        currentFile.close();
    }

    throw std::runtime_error(errMsg);
}

void x86_64elf::createStringTables() {
    for (int i = 0; i < eHdr->e_shnum; ++i) {
        if (sectionHeaders[i].sh_type == SHT_STRTAB) {
            std::vector<char> strTable;
            strTable.resize(sectionHeaders[i].sh_size);

            currentFile.seekg(static_cast<long>(sectionHeaders[i].sh_offset), std::ios::beg);
            currentFile.read(strTable.data(), static_cast<long>(sectionHeaders[i].sh_size));

            if (!currentFile.good()) {
                handeFileError("Could not read string table");
            }
            stringTables[i] = strTable;
        }
    }
}

void x86_64elf::createSymbolTables() {
    for (int i = 0; i < eHdr->e_shnum; ++i) {
        if (sectionHeaders[i].sh_type == SHT_SYMTAB || sectionHeaders[i].sh_type == SHT_DYNSYM) {
            std::vector<Elf64_Sym> symbols;
            symbols.resize(sectionHeaders[i].sh_size / sizeof(Elf64_Sym));

            currentFile.seekg(static_cast<long>(sectionHeaders[i].sh_offset), std::ios::beg);
            currentFile.read(reinterpret_cast<std::istream::char_type *>(symbols.data()),
                             static_cast<long>(sectionHeaders[i].sh_size));
            if (!currentFile.good()) {
                handeFileError("Could not read symbols");
            }

            symbolTables[i] = symbols;
            for (const auto &symbol: symbols) {
                if (symbol.st_shndx == SHN_UNDEF)
                    continue;

                if (const auto type = ELF64_ST_TYPE(symbol.st_info);
                    type != STT_FUNC && type != STT_OBJECT && type != STT_NOTYPE)
                    continue;

                if (symbol.st_value == 0 && symbol.st_shndx != SHN_ABS)
                    continue;

                symbolAddressTable[symbol.st_value] = demangler->demangle(
                    &stringTables[sectionHeaders[i].sh_link][symbol.st_name]);
            }
        } else if (sectionHeaders[i].sh_type == SHT_RELA) {
            std::vector<Elf64_Rela> relocations(sectionHeaders[i].sh_size / sizeof(Elf64_Rela));

            currentFile.seekg(static_cast<long>(sectionHeaders[i].sh_offset), std::ios::beg);
            currentFile.read(reinterpret_cast<std::istream::char_type *>(relocations.data()),
                             static_cast<long>(sectionHeaders[i].sh_size));
            if (!currentFile.good()) {
                handeFileError("Could not read relocations");
            }

            for (const auto &relocation: relocations) {
                const uint32_t symIndex = ELF64_R_SYM(relocation.r_info);
                const uint32_t type = ELF64_R_TYPE(relocation.r_info);
                relaTables[i].emplace_back(relocation);

                const auto syms = symbolTables[sectionHeaders[i].sh_link][symIndex];
                uint32_t strtabIndex = sectionHeaders[sectionHeaders[i].sh_link].sh_link;


                symbolAddressTable[relocation.r_offset] = demangler->demangle(&stringTables[strtabIndex][syms.st_name]);
            }
        }
    }
}

x86_64elf::~x86_64elf() = default;

std::vector<std::pair<uint64_t, std::string> > x86_64elf::getSectionHeadersNames() {
    std::vector<std::pair<uint64_t, std::string> > names;

    for (const auto &[k,v]: segmentsIndex) {
        names.emplace_back(v.sh_addr, k);
    }
    return names;
}

std::vector<std::string> x86_64elf::getSectionNames() {
    std::vector<std::string> names;

    for (const auto &section: sectionHeaders) {
        names.emplace_back(&stringTable[section.sh_name]);
    }

    return names;
}

std::vector<std::pair<std::string, std::vector<char> > > x86_64elf::getStringTables() {
    std::vector<std::pair<std::string, std::vector<char> > > erg;

    for (const auto &[index,data]: stringTables) {
        std::string test = &stringTable[sectionHeaders[index].sh_name];
        erg.emplace_back(test, data);
    }

    return erg;
}

std::vector<std::pair<std::string, std::pair<std::string, Elf64_Sym> > > x86_64elf::getSymbolTablesElf64() {
    std::vector<std::pair<std::string, std::pair<std::string, Elf64_Sym> > > symbols;

    for (const auto &[index,data]: symbolTables) {
        for (const auto &symbol: data) {
            symbols.emplace_back(&stringTable[sectionHeaders[index].sh_name],
                                 std::make_pair(&stringTables[sectionHeaders[index].sh_link][symbol.st_name], symbol));
        }
    }

    return symbols;
}

Elf64_Ehdr x86_64elf::getElf64Header() {
    return *eHdr;
}

std::unordered_map<std::string, std::pair<std::string, std::vector<std::pair<std::string, Elf64_Rela> > > >
x86_64elf::getRelaTables64() {
    std::unordered_map<std::string, std::pair<std::string, std::vector<std::pair<std::string, Elf64_Rela> > > > erg;

    for (const auto &[index,data]: relaTables) {
        auto table = std::string(&stringTable[sectionHeaders[index].sh_name]);
        std::vector<std::pair<std::string, Elf64_Rela> > list;
        for (const auto &symbol: data) {
            const auto syms = symbolTables[sectionHeaders[index].sh_link][ELF64_R_SYM(symbol.r_info)];
            uint32_t strtabIndex = sectionHeaders[sectionHeaders[index].sh_link].sh_link;
            list.emplace_back(&stringTables[strtabIndex][syms.st_name], symbol);
        }
        erg[table] = std::make_pair(&stringTable[sectionHeaders[sectionHeaders[index].sh_link].sh_name], list);
    }

    return erg;
}

std::vector<uint8_t> x86_64elf::getSection(const std::string &sectionName) {
    std::vector<uint8_t> data;

    const auto hit = segmentsIndex.find(sectionName);
    if (hit == segmentsIndex.end())
        return data;

    const auto header = hit->second;

    data.resize(header.sh_size);
    std::lock_guard<std::mutex> lock(fileMutex);
    currentFile.seekg(static_cast<long>(header.sh_offset), std::ios::beg);
    currentFile.read(reinterpret_cast<std::istream::char_type *>(data.data()),
                     static_cast<long>(header.sh_size));
    return data;
}

std::string x86_64elf::lookupRangeSymbol(const uint64_t addr) {
    auto symbol = symbolAddressTable.upper_bound(addr);
    if (symbol == symbolAddressTable.begin())
        return "";

    --symbol;

    const uint64_t offset = addr - symbol->first;
    if (offset == 0)
        return symbol->second;

    return symbol->second + "+0x" + std::format("{:x}", offset);
}

uint64_t x86_64elf::getAddressOfSegment(const std::string &segmentName) {
    const auto hit = segmentsIndex.find(segmentName);
    if (hit == segmentsIndex.end())
        return 0;

    return hit->second.sh_addr;
}

std::string x86_64elf::lookupSymbol(const uint64_t addr) {
    const auto symbol = symbolAddressTable.find(addr);
    return symbol != symbolAddressTable.end() ? symbol->second : "";
}

std::vector<std::pair<uint64_t, uint64_t> > x86_64elf::getLoadableProgramHeaders() {
    if (programHeaders.empty())
        return {};

    std::vector<std::pair<uint64_t, uint64_t> > erg;

    for (const auto &header: programHeaders) {
        if (header.p_type != PT_LOAD)
            continue;

        erg.emplace_back(header.p_vaddr, header.p_memsz);
    }

    return erg;
}

std::vector<std::pair<uint64_t, uint64_t> > x86_64elf::getLoadableSectionHeaders() {
    if (sectionHeaders.empty())
        return {};

    std::vector<std::pair<uint64_t, uint64_t> > erg;
    for (const auto &header: sectionHeaders) {
        if (header.sh_flags & SHF_ALLOC)
            erg.emplace_back(header.sh_addr, header.sh_size);
    }

    return erg;
}
