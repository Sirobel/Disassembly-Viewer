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
            oss << "section <" << sectionName << ">" << std::endl;
        }
        oss << "\t";

        oss << "0x" << std::right << std::hex << insn[i].address << ":    ";
        std::ostringstream bytes;
        for (int j = 0; j < insn[i].size; ++j) {
            bytes << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(insn[i].bytes[j]) << " ";
        }

        oss << bytes.str() << std::left << std::setfill(' ') << std::setw(12) << (
            "\u200c" + std::string(insn[i].mnemonic) + "\u200c") << std::setw(12) << insn[i].op_str;

        for (int j = 0; j < detail->x86.op_count; j++) {
            const auto &op = detail->x86.operands[j];

            if (op.type == X86_OP_MEM && op.mem.base == X86_REG_RIP) {
                oss << "\t#target ";
                uint64_t targetAddress = insn[i].address + insn[i].size + op.mem.disp;
                oss << "0x" << std::hex << targetAddress << " <" << elf.lookupRangeSymbol(targetAddress) << ">";
                break;
            }
            if (hasJCOperation) {
                oss << "\t#target ";
                if (op.type == X86_OP_IMM) {
                    oss << "0x" << std::hex << op.imm << " <" << elf.lookupRangeSymbol(op.imm) << ">";
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

    return out;
}

x86_64Disasm::~x86_64Disasm() = default;

QVector<DisasmModel::Function> x86_64Disasm::disassemblePartToSections(
    const std::vector<uint8_t> &machineCode, uint64_t startingAddress) {
    QVector<DisasmModel::Function> sections;
    DisasmModel::Function current;

    csh handle;
    cs_insn *insn;
    cs_open(CS_ARCH_X86, CS_MODE_64, &handle);
    cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);

    const size_t count = cs_disasm(handle, machineCode.data(), machineCode.size(), startingAddress, 0, &insn);

    for (size_t i = 0; i < count; i++) {
        const cs_detail *detail = insn[i].detail;

        if (auto sym = elf.lookupSymbol(insn[i].address); !sym.empty()) {
            if (!current.instructions.isEmpty())
                sections.append(current);
            current = {QString::fromStdString("section <"+sym+">"), {}};
        }

        std::ostringstream bytes;
        for (int j = 0; j < insn[i].size; ++j)
            bytes << std::hex << std::setw(2) << std::setfill('0')
                    << static_cast<int>(insn[i].bytes[j]) << " ";

        QString comment;
        const bool hasJC = cs_insn_group(handle, &insn[i], CS_GRP_JUMP)
                     || cs_insn_group(handle, &insn[i], CS_GRP_CALL);

        for (int j = 0; j < detail->x86.op_count; j++) {
            const auto &op = detail->x86.operands[j];
            if (op.type == X86_OP_MEM && op.mem.base == X86_REG_RIP) {
                const uint64_t target = insn[i].address + insn[i].size + op.mem.disp;
                comment = QString("#target 0x%1 <%2>")
                        .arg(target, 0, 16)
                        .arg(QString::fromStdString(elf.lookupRangeSymbol(target)));
                break;
            }
            if (hasJC) {
                if (op.type == X86_OP_IMM)
                    comment = QString("#target 0x%1 <%2>")
                            .arg(static_cast<uint64_t>(op.imm), 0, 16)
                            .arg(QString::fromStdString(elf.lookupRangeSymbol(op.imm)));
                else if (op.type == X86_OP_REG)
                    comment = QString::fromUtf8(cs_reg_name(handle, op.reg));
                break;
            }
        }

        current.instructions.append({
            QString("0x%1").arg(insn[i].address, 0, 16),
            QString::fromStdString(bytes.str()),
            QString::fromUtf8(insn[i].mnemonic),
            QString::fromUtf8(insn[i].op_str),
            comment
        });
    }

    if (!current.instructions.isEmpty())
        sections.append(current);

    cs_free(insn, count);
    cs_close(&handle);
    return sections;
}
