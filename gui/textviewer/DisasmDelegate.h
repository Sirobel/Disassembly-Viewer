//
// Created by finns on 09.05.26.
//

#ifndef DISASSEMBLY_VIEWER_DISASMDELEGATE_H
#define DISASSEMBLY_VIEWER_DISASMDELEGATE_H

#include <QStyledItemDelegate>
#include <QTreeView>

class DisasmDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit DisasmDelegate(QTreeView *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
private:
    QTreeView *view;
};

#endif //DISASSEMBLY_VIEWER_DISASMDELEGATE_H
