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
        return sections.size();
    }

    if (parent.internalId() == static_cast<quintptr>(-1)) {
        return sections[parent.row()].instructions.size();
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

    const auto &[address, bytes, mnemonic, operands, comment] = sections[index.internalId()].instructions[index.row()];
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
