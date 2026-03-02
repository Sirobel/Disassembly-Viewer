//
// Created by finns on 25.02.26.
//

#include "x86_64Disasm.h"

#include <iomanip>
#include <capstone/capstone.h>
#include <sstream>

std::string x86_64Disasm::disassemblePart(const std::vector<uint8_t> &machineCode, uint64_t startingAddress) {
    std::string out;


    csh handle;
    cs_insn *insn;
    cs_open(CS_ARCH_X86, CS_MODE_64, &handle);

    cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
    const size_t count = cs_disasm(handle, machineCode.data(), machineCode.size(), startingAddress, 0, &insn);
    for (size_t i = 0; i < count; i++) {
        cs_detail *detail = insn[i].detail;
        std::ostringstream oss;

        oss <<std::right<<std::setw(6) << std::setfill('0') << std::hex << insn[i].address << ":    ";
        std::ostringstream bytes;
        for (int j = 0; j < insn[i].size; ++j) {
            bytes << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(insn[i].bytes[j]) << " ";
        }
        oss << std::left << std::setfill(' ') << std::setw(12) << insn[i].mnemonic << std::setw(12) << insn[i].op_str;
        oss << std::endl;
        out += oss.str();
    }
    cs_free(insn, count);
    cs_close(&handle);

    return out;
}

x86_64Disasm::~x86_64Disasm() = default;
