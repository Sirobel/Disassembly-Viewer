//
// Created by finns on 25.02.26.
//

#ifndef DISASSEMBLY_VIEWER_DISASSEMBLER_H
#define DISASSEMBLY_VIEWER_DISASSEMBLER_H

#pragma once
#include <ElfHandler.h>

class Disassembler {
public:
    virtual ~Disassembler() = default;

    explicit Disassembler(const ElfHandler &elfHandler) : elf(elfHandler) {
    }

    virtual std::string disassemblePart(const std::vector<uint8_t> &machineCode, uint64_t startingAddress) {
        return "";
    }



private:
    ElfHandler elf;
};


#endif //DISASSEMBLY_VIEWER_DISASSEMBLER_H
