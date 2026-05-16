//
// Created by finns on 25.02.26.
//

#ifndef DISASSEMBLY_VIEWER_X86_64DISASM_H
#define DISASSEMBLY_VIEWER_X86_64DISASM_H
#include "Disassembler.h"


class x86_64Disasm : public Disassembler {
public:
    explicit x86_64Disasm(ElfHandler &elfHandler)
        : Disassembler(elfHandler) {
    }

    std::string disassemblePart(const std::vector<uint8_t> &machineCode, uint64_t startingAddress) override;

    ~x86_64Disasm() override;

    QVector<DisasmModel::Function> disassemblePartToSections(const std::vector<uint8_t> &machineCode,
                                                        uint64_t startingAddress) const override;

private:
    std::string baseUrl = "https://www.felixcloutier.com/x86/";
};


#endif //DISASSEMBLY_VIEWER_X86_64DISASM_H
