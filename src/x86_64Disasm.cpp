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
        bool hasJCOperation = false;
        if (cs_insn_group(handle, &insn[i], CS_GRP_JUMP) || cs_insn_group(handle, &insn[i], CS_GRP_CALL)) {
            hasJCOperation = true;
        }

        if (auto sectionName = elf.lookupSymbol(insn[i].address); !sectionName.empty()) {
            oss << "section &lt;" << sectionName << "&gt;" << std::endl;
        }
        oss << "\t";

        oss << "0x" << std::right << std::setw(6) << std::setfill('0') << std::hex << insn[i].address << ":    ";
        std::ostringstream bytes;
        for (int j = 0; j < insn[i].size; ++j) {
            bytes << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(insn[i].bytes[j]) << " ";
        }

        oss << "<a href=\"" << baseUrl << insn[i].mnemonic << "\">" << std::left << std::setfill(' ') << std::setw(12)
                << insn[i].mnemonic << "</a>" << std::setw(12) << insn[i].op_str;

        for (int j = 0; j < detail->x86.op_count; j++) {
            const auto &op = detail->x86.operands[j];

            if (op.type == X86_OP_MEM && op.mem.base == X86_REG_RIP) {
                oss << "\t#target ";
                uint64_t targetAddress = insn[i].address + insn[i].size + op.mem.disp;
                oss << "<a href=\"0x" << std::hex << std::right << std::setw(6) << std::setfill('0') << targetAddress <<
                        "\">0x";
                oss << std::hex << targetAddress << " &lt;" << elf.lookupRangeSymbol(targetAddress) << "&gt;";
                oss << "</a>";
                break;
            }
            if (hasJCOperation) {
                oss << "\t#target ";
                if (op.type == X86_OP_IMM) {
                    oss << "<a href=\"0x" << std::hex << std::right << std::setw(6) << std::setfill('0') << op.imm <<
                            "\">0x" << std::hex << op.imm << "&lt;" << elf.lookupRangeSymbol(op.imm) <<
                            "&gt;" << "</a>";
                    break;
                }
                if (op.type == X86_OP_REG) {
                    oss << cs_reg_name(handle, op.reg);
                    break;
                }
            }
        }

        oss << std::endl;
        out += oss.str();
    }
    cs_free(insn, count);
    cs_close(&handle);

    return "<pre>" + out + "</pre>";
}

x86_64Disasm::~x86_64Disasm() = default;
