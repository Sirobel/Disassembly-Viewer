//
// Created by finns on 25.02.26.
//

#ifndef DISASSEMBLY_VIEWER_DISASSEMBLER_H
#define DISASSEMBLY_VIEWER_DISASSEMBLER_H

#pragma once
#include <ElfHandler.h>
#include <../gui/textviewer/tree/DisasmModel.h>

class Disassembler {
public:
    virtual ~Disassembler() = default;

    explicit Disassembler(ElfHandler &elfHandler) : elf(elfHandler) {
    }

    virtual std::string disassemblePart(const std::vector<uint8_t> &machineCode, uint64_t startingAddress) {
        return "";
    }

    virtual QVector<DisasmModel::Function> disassemblePartToSections(const std::vector<uint8_t> &machineCode,
                                                                    uint64_t startingAddress) const {
        return {};
    }

protected:
    ElfHandler &elf;
};


#endif //DISASSEMBLY_VIEWER_DISASSEMBLER_H
