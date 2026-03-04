//
// Created by finns on 21.02.26.
//

#ifndef DISASSEMBLY_VIEWER_X86_64ELF_H
#define DISASSEMBLY_VIEWER_X86_64ELF_H
#include "ElfHandler.h"
#include <elf.h>
#include <map>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>


class x86_64elf : public ElfHandler {
public:
    explicit x86_64elf(const std::string &path);

private:
    std::ifstream currentFile;
    std::unique_ptr<Elf64_Ehdr> eHdr;
    std::vector<Elf64_Shdr> sectionHeaders;
    std::vector<Elf64_Phdr> programHeaders;
    std::vector<char> stringTable;
    std::unordered_map<uint32_t, std::vector<char> > stringTables;
    std::unordered_map<uint32_t, std::vector<Elf64_Sym> > symbolTables;
    std::map<uint64_t, std::string> symbolAddressTable;
    std::unordered_map<std::string, Elf64_Shdr> segmentsIndex;
    std::mutex fileMutex;

    void handeFileError(const std::string &errMsg);

    void createStringTables();

    void createSymbolTables();

public:
    ~x86_64elf() override;

    std::vector<std::string> getSectionHeadersNames() override;

    std::vector<uint8_t> getSection(const std::string &sectionName) override;

    std::string lookupRangeSymbol(uint64_t addr) override;

    uint64_t getAddressOfSegment(const std::string &segmentName) override;

    std::string lookupSymbol(uint64_t addr) override;
};


#endif //DISASSEMBLY_VIEWER_X86_64ELF_H
