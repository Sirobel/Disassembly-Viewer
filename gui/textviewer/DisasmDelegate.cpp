//
// Created by finns on 09.05.26.
//

#include "DisasmDelegate.h"
#include <QPainter>
#include <QTreeView>
#include <QHeaderView>
#include <QMouseEvent>
#include <QEvent>


DisasmDelegate::DisasmDelegate(QTreeView *parent) : QStyledItemDelegate(parent),
                                                    settings("Sirobel", "Disassembly-Viewer") {
}

void DisasmDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const {
    if (!index.parent().isValid() && index.column() == 0) {
        auto *view = qobject_cast<const QTreeView *>(option.widget);
        if (view) {
            QRect spanRect = option.rect;
            int totalWidth = 0;
            for (int c = 0; c < view->header()->count(); ++c)
                totalWidth += view->header()->sectionSize(c);
            spanRect.setWidth(totalWidth - spanRect.x() + view->header()->offset());

            QStyleOptionViewItem opt = option;
            opt.rect = spanRect;
            initStyleOption(&opt, index);

            // Save state, disable clipping, paint, restore
            painter->save();
            painter->setClipping(false);
            QStyledItemDelegate::paint(painter, opt, index);
            painter->restore();
            return;
        }
    }

    if (!index.parent().isValid() && index.column() != 0) {
        return;
    }

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
