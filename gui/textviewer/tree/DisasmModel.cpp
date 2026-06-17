//
// Created by finns on 07.05.26.
//

#include "../tree/DisasmModel.h"

#include <QColor>

DisasmModel::DisasmModel(QObject *parent) : QAbstractItemModel(parent), settings("Sirobel", "Disassembly-Viewer") {
    linkColor = QColor(settings.value("linkColor", "#0000EE").toString());
    textColor = QColor(settings.value("textColor", "#ffffff").toString());
}

/**
 * Bits 63-48 section index +1
 * Bits 47-32 function index
 * Bits 31-0 instruction index
 */

quintptr DisasmModel::encodeId(int sec, int func, int insn) const {
    // sec/func = -1 means "not applicable at this level"
    // We store (sec+1) so 0 means "root child"
    return (static_cast<quintptr>(sec + 1) << 48)
           | (static_cast<quintptr>(func + 1) << 32)
           | static_cast<quintptr>(insn + 1);
}

void DisasmModel::decodeId(quintptr id, int &sec, int &func, int &insn) const {
    sec = static_cast<int>((id >> 48) & 0xFFFF) - 1;
    func = static_cast<int>((id >> 32) & 0xFFFF) - 1;
    insn = static_cast<int>(id & 0xFFFFFFFF) - 1;
}

QModelIndex DisasmModel::index(int row, int col, const QModelIndex &parent) const {
    if (!parent.isValid()) {
        // Root → section level
        return createIndex(row, col, encodeId(row, -1, -1));
    }

    int sec, func, insn;
    decodeId(parent.internalId(), sec, func, insn);

    if (func == -1) {
        // Section level → function level
        return createIndex(row, col, encodeId(sec, row, -1));
    } else {
        // Function level → instruction level
        return createIndex(row, col, encodeId(sec, func, row));
    }
}

QModelIndex DisasmModel::parent(const QModelIndex &child) const {
    if (!child.isValid()) return {};

    int sec, func, insn;
    decodeId(child.internalId(), sec, func, insn);

    if (func == -1) return {}; // section → root
    if (insn == -1)
        return createIndex(sec, 0, // function → section
                           encodeId(sec, -1, -1));
    return createIndex(func, 0, encodeId(sec, func, -1)); // insn → function
}

int DisasmModel::rowCount(const QModelIndex &parent) const {
    if (!parent.isValid())
        return static_cast<int>(sections.size());

    int sec, func, insn;
    decodeId(parent.internalId(), sec, func, insn);

    if (func == -1) return static_cast<int>(sections[sec].functions.size());
    if (insn == -1) return static_cast<int>(sections[sec].functions[func].instructions.size());
    return 0; // instructions have no children
}

int DisasmModel::columnCount(const QModelIndex &parent) const {
    return 5;
}

QVariant DisasmModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::ToolTipRole)) return {};

    int sec, func, insn;
    decodeId(index.internalId(), sec, func, insn);

    if (func == -1) {
        // Section row
        if (index.column() == 0)
            return sections[sec].name;
        return {};
    }
    if (insn == -1) {
        // Function row
        const auto &[name, instructions] = sections[sec].functions[func];
        if (index.column() == 0)
            return name;
        return {};
    }

    // Instruction row
    const auto &[address, bytes, mnemonic, operands, comment] = sections[sec].functions[func].instructions[insn];
    switch (index.column()) {
        case 0: return address;
        case 1: return bytes;
        case 2: return mnemonic;
        case 3: return operands;
        case 4: return comment;
        default:
            return {};
    }
}

void DisasmModel::setSections(QVector<Section> data) {
    beginResetModel();
    indexes.clear();
    sections = std::move(data);
    endResetModel();

    for (int s = 0; s < sections.size(); ++s) {
        for (int f = 0; f < sections[s].functions.size(); ++f) {
            for (int i = 0; i < sections[s].functions[f].instructions.size(); ++i) {
                auto &insn = sections[s].functions[f].instructions[i];
                indexes.insert(insn.address, createIndex(i, 0, encodeId(s, f, i)));
            }
        }
    }
}

QModelIndex DisasmModel::findAddress(const QString &address) const {
    return indexes.value(address, {});
}

static bool instructionMatches(const DisasmModel::Instruction &insn, const QString &text) {
    return insn.address.contains(text, Qt::CaseInsensitive) ||
           insn.bytes.contains(text, Qt::CaseInsensitive) ||
           insn.mnemonic.contains(text, Qt::CaseInsensitive) ||
           insn.operands.contains(text, Qt::CaseInsensitive) ||
           insn.comment.contains(text, Qt::CaseInsensitive);
}

QModelIndex DisasmModel::findNext(const QString &text, const QModelIndex &from) {
    if (text.isEmpty() || sections.isEmpty())
        return {};

    int sec = -1, func = -1, insn = -1;
    if (from.isValid())
        decodeId(from.internalId(), sec, func, insn);

    for (int s = (sec >= 0) ? sec : 0; s < sections.size(); ++s) {
        const auto &[SName, functions] = sections[s];
        if (s > sec || (func == -1 && insn == -1))
            if ((sec == -1 || s > sec) && SName.contains(text, Qt::CaseInsensitive))
                return createIndex(s, 0, encodeId(s, -1, -1));

        for (int f = (s == sec && func >= 0) ? func : 0; f < functions.size(); ++f) {
            const auto &[fName, instructions] = functions[f];
            if (f > func || (f == func && s > sec))
                if (fName.contains(text, Qt::CaseInsensitive))
                    return createIndex(f, 0, encodeId(s, f, -1));

            for (int i = (s == sec && f == func && insn >= 0) ? insn + 1 : 0; i < instructions.size(); ++i) {
                if (instructionMatches(instructions[i], text))
                    return createIndex(i, 0, encodeId(s, f, i));
            }
        }
    }

    return {};
}

QModelIndex DisasmModel::findPrev(const QString &text, const QModelIndex &from) {
    if (text.isEmpty() || sections.isEmpty())
        return {};

    int sec = -1, func = -1, insn = -1;
    if (from.isValid())
        decodeId(from.internalId(), sec, func, insn);

    for (int s = (sec >= 0) ? sec : static_cast<int>(sections.size()) - 1; s >= 0; --s) {
        const auto &[SName, functions] = sections[s];

        for (int f = (func >= -1 && sec == s) ? func : static_cast<int>(functions.size()) - 1; f >= 0; --f) {
            const auto &[fName, instructions] = functions[f];
            for (int i = (insn >= -1 && sec == s && func == f) ? insn - 1 : static_cast<int>(instructions.size()) - 1;
                 i >= 0; --i) {
                if (instructionMatches(instructions[i], text))
                    return createIndex(i, 0, encodeId(s, f, i));
            }

            if (!(s == sec && f == func && insn == -1))
                if (fName.contains(text, Qt::CaseInsensitive))
                    return createIndex(f, 0, encodeId(s, f, -1));
        }

        if (!(s == sec && func == -1 && insn == -1))
            if (SName.contains(text, Qt::CaseInsensitive))
                return createIndex(s, 0, encodeId(s, -1, -1));
    }

    return {};
}

int DisasmModel::countMatches(const QString &text) {
    if (text.isEmpty()) return 0;

    int count = 0;
    for (auto &[secName, functions]: sections) {
        if (secName.contains(text, Qt::CaseInsensitive))
            count++;
        for (auto &[funcName, instructions]: functions) {
            if (funcName.contains(text, Qt::CaseInsensitive))
                count++;
            for (auto &insn: instructions) {
                if (instructionMatches(insn, text))
                    count++;
            }
        }
    }

    return count;
}
