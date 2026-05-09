//
// Created by finns on 07.05.26.
//

#ifndef DISASSEMBLY_VIEWER_DISASMMODEL_H
#define DISASSEMBLY_VIEWER_DISASMMODEL_H
#include <qabstractitemmodel.h>
#include <QSettings>
#include <QColor>


class DisasmModel : public QAbstractItemModel {
    Q_OBJECT

public:
    struct Instruction {
        QString address, bytes, mnemonic, operands, comment;
    };

    struct Section {
        QString name;
        QVector<Instruction> instructions;
    };

    explicit DisasmModel(QObject *parent = nullptr);

    QModelIndex index(int row, int col, const QModelIndex &parent) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setSections(QVector<Section> data);

    QModelIndex findAddress(const QString &address) const;

private:
    QVector<Section> sections;
    QSettings settings;
    QColor linkColor;
    QColor textColor;
    QHash<QString, QModelIndex> indexes;
};


#endif //DISASSEMBLY_VIEWER_DISASMMODEL_H
