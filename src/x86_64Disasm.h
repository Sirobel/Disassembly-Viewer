//
// Created by finns on 25.02.26.
//

#ifndef DISASSEMBLY_VIEWER_X86_64DISASM_H
#define DISASSEMBLY_VIEWER_X86_64DISASM_H
#include "Disassembler.h"


class x86_64Disasm : public Disassembler{
public:
    explicit x86_64Disasm(const ElfHandler &elfHandler)
        : Disassembler(elfHandler) {
    }


    std::string disassemblePart(const std::vector<uint8_t> &machineCode, uint64_t startingAddress) override;

    ~x86_64Disasm() override;

};


#endif //DISASSEMBLY_VIEWER_X86_64DISASM_H