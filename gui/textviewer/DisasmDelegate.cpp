//
// Created by finns on 09.05.26.
//

#include "DisasmDelegate.h"
#include <QPainter>
#include <QTreeView>
#include <QHeaderView>
#include <QMouseEvent>
#include <QEvent>


DisasmDelegate::DisasmDelegate(QTreeView *parent) : QStyledItemDelegate(parent), view(parent) {
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

