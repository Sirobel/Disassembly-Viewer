//
// Created by finns on 09.05.26.
//

#ifndef DISASSEMBLY_VIEWER_DISASMDELEGATE_H
#define DISASSEMBLY_VIEWER_DISASMDELEGATE_H

#include <QStyledItemDelegate>
#include <QTreeView>
#include <qsettings.h>

class DisasmDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit DisasmDelegate(QTreeView *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    void refresh();

protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override;

private:
    QTreeView *view;
    QSettings settings;
    QColor textColor;
    QColor linkColor;
    bool showUnderline;

    static int getDepth(const QModelIndex &index);
};

#endif //DISASSEMBLY_VIEWER_DISASMDELEGATE_H
