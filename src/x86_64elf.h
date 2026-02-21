//
// Created by finns on 21.02.26.
//

#ifndef DISASSEMBLY_VIEWER_X86_64ELF_H
#define DISASSEMBLY_VIEWER_X86_64ELF_H
#include "ElfHandler.h"
#include <elf.h>
#include <memory>
#include <vector>


class x86_64elf : public ElfHandler {
public:
    explicit x86_64elf(const std::string &path);

private:
    std::ifstream currentFile;
    std::unique_ptr<Elf64_Ehdr> eHdr;
    std::vector<Elf64_Shdr> sectionHeaders;
    std::vector<Elf64_Phdr> programHeaders;
    std::vector<char> stringTable;


    void handeFileError(const std::string &errMsg);

public:
    ~x86_64elf() override;

    std::vector<std::string> getSectionHeadersNames() override;
};


#endif //DISASSEMBLY_VIEWER_X86_64ELF_H
