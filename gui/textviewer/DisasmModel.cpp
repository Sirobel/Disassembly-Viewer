//
// Created by finns on 07.05.26.
//

#include "DisasmModel.h"
#include <QColor>

DisasmModel::DisasmModel(QObject *parent) : settings("Sirobel", "Disassembly-Viewer") {
    linkColor = QColor(settings.value("linkColor", "#0000EE").toString());
    textColor = QColor(settings.value("textColor", "#ffffff").toString());
}

QModelIndex DisasmModel::index(int row, int col, const QModelIndex &parent) const {
    if (!parent.isValid()) {
        return createIndex(row, col, static_cast<quintptr>(-1));
    }

    return createIndex(row, col, static_cast<quintptr>(parent.row()));
}

QModelIndex DisasmModel::parent(const QModelIndex &child) const {
    if (!child.isValid() || child.internalId() == static_cast<quintptr>(-1)) {
        return {};
    }

    return createIndex(static_cast<int>(child.internalId()), 0, static_cast<quintptr>(-1));
}

int DisasmModel::rowCount(const QModelIndex &parent) const {
    if (!parent.isValid()) {
        return static_cast<int>(sections.size());
    }

    if (parent.internalId() == static_cast<quintptr>(-1)) {
        return static_cast<int>(sections[parent.row()].instructions.size());
    }
    return 0;
}

int DisasmModel::columnCount(const QModelIndex &parent) const {
    return 5;
}

QVariant DisasmModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return {};
    }

    if (index.internalId() == static_cast<quintptr>(-1)) {
        if (role == Qt::DisplayRole && index.column() == 0) {
            return sections[index.row()].name;
        }
        return {};
    }

    const auto &[address, bytes, mnemonic, operands, comment] = sections[static_cast<int>(index.internalId())].
            instructions[index.row()];
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return address;
            case 1:
                return bytes;
            case 2:
                return mnemonic;
            case 3:
                return operands;
            case 4:
                return comment;
            default: ;
        }
    }

    return {};
}

void DisasmModel::setSections(QVector<Section> data) {
    beginResetModel();
    sections = std::move(data);

    indexes.clear();

    for (int i = 0; i < sections.size(); i++) {
        const auto &insn = sections[i].instructions;
        for (int j = 0; j < insn.size(); ++j) {
            indexes.insert(sections[i].instructions[j].address, createIndex(j, 0, static_cast<quintptr>(i)));
        }
    }

    endResetModel();
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
    if (text.isEmpty()) return {};

    int startSection = 0;
    int startRow = -2;

    if (from.isValid()) {
        if (from.internalId() == static_cast<quintptr>(-1)) {

            startSection = from.row();
            startRow = -1;
        } else {
            startSection = static_cast<int>(from.internalId());
            startRow = from.row();
        }
    }

    for (int si = startSection; si < sections.size(); ++si) {
        if (si == startSection ? startRow == -2 : true) {
            if (sections[si].name.contains(text, Qt::CaseInsensitive))
                return createIndex(si, 0, static_cast<quintptr>(-1));
        }

        const auto &insns = sections[si].instructions;
        int rowStart = (si == startSection) ? startRow + 1 : 0;
        for (int ri = rowStart; ri < insns.size(); ++ri) {
            if (instructionMatches(insns[ri], text))
                return createIndex(ri, 0, static_cast<quintptr>(si));
        }
    }
    return {};
}

QModelIndex DisasmModel::findPrev(const QString &text, const QModelIndex &from) {
    if (text.isEmpty()) return {};

    int startSection = static_cast<int>(sections.size()) - 1;
    int startRow = sections.isEmpty() ? -1 : static_cast<int>(sections.back().instructions.size());

    if (from.isValid()) {
        if (from.internalId() == static_cast<quintptr>(-1)) {
            startSection = from.row() - 1;
            startRow = startSection >= 0 ? static_cast<int>(sections[startSection].instructions.size()) : -1;
        } else {
            startSection = static_cast<int>(from.internalId());
            startRow = from.row();
        }
    }

    for (int si = startSection; si >= 0; --si) {
        const auto &insns = sections[si].instructions;
        const int rowStart = (si == startSection) ? startRow - 1 : insns.size() - 1;
        for (int ri = rowStart; ri >= 0; --ri) {
            if (instructionMatches(insns[ri], text))
                return createIndex(ri, 0, static_cast<quintptr>(si));
        }

        if (sections[si].name.contains(text, Qt::CaseInsensitive))
            return createIndex(si, 0, static_cast<quintptr>(-1));
    }
    return {};
}

int DisasmModel::countMatches(const QString &text) {
    if (text.isEmpty()) {
        return 0;
    }

    int count = 0;
    for (const auto &[name, instructions]: sections) {
        if (name.contains(text, Qt::CaseInsensitive))
            count++;
        for (const auto &[address, bytes, mnemonic, operands, comment]: instructions) {
            if (address.contains(text, Qt::CaseInsensitive) ||
                mnemonic.contains(text, Qt::CaseInsensitive) ||
                operands.contains(text, Qt::CaseInsensitive) ||
                comment.contains(text, Qt::CaseInsensitive) ||
                bytes.contains(text, Qt::CaseInsensitive))
                count++;
        }
    }

    return count;
}
