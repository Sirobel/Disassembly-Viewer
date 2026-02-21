//
// Created by finns on 21.02.26.
//

#include "x86_64elf.h"

#include <cstring>
#include <iostream>
#include <memory>
#include <vector>

x86_64elf::x86_64elf(const std::string &path) : ElfHandler(path) {
    currentFile.open(path);

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
    if (sectionHeaders[eHdr->e_shstrndx].sh_type != SHT_STRTAB) {
        handeFileError("Wrong entry for e_shstrndx");
    }

    stringTable.resize(sectionHeaders[eHdr->e_shstrndx].sh_size);
    currentFile.seekg(static_cast<long>(sectionHeaders[eHdr->e_shstrndx].sh_offset), std::ios::beg);
    currentFile.read(stringTable.data(), static_cast<long>(sectionHeaders[eHdr->e_shstrndx].sh_size));

    if (!currentFile.good())  {
        handeFileError("Could not read stringTable headers");
    }
}

void x86_64elf::handeFileError(const std::string &errMsg) {
    if (currentFile.is_open()) {
        currentFile.close();
    }

    throw std::runtime_error(errMsg);
}

x86_64elf::~x86_64elf() = default;

std::vector<std::string> x86_64elf::getSectionHeadersNames() {
    std::vector<std::string> names;

    for (const auto &header: sectionHeaders) {
        names.emplace_back(&stringTable[header.sh_name]);
    }
    return names;
}
