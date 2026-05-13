//
// Created by finns on 09.05.26.
//

#include "../tree/DisasmDelegate.h"
#include <QPainter>
#include <QTreeView>
#include <QHeaderView>
#include <QMouseEvent>
#include <QEvent>


DisasmDelegate::DisasmDelegate(QTreeView *parent) : QStyledItemDelegate(parent), view(parent),
                                                    settings("Sirobel", "Disassembly-Viewer") {
}

void DisasmDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);
}

void DisasmDelegate::refresh() {
    linkColor = QColor(settings.value("linkColor").toString());
    textColor = QColor(settings.value("textColor").toString());
    showUnderline = settings.value("linkUnderscore", Qt::Checked).toInt() == Qt::Checked;
}

void DisasmDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const {
    QStyledItemDelegate::initStyleOption(option, index);

    if (index.column() == 2 || index.column() == 4) {
        option->palette.setColor(QPalette::Text, linkColor);
        if (showUnderline)
            option->font.setUnderline(true);
    } else {
        option->palette.setColor(QPalette::Text, textColor);
    }
}

int DisasmDelegate::getDepth(const QModelIndex &index) {
    int depth = 0;
    QModelIndex p = index.parent();
    while (p.isValid()) {
        ++depth;
        p = p.parent();
    }
    return depth;
}
