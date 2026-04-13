//
// Created by finns on 09.04.26.
//

#ifndef DISASSEMBLY_VIEWER_TREEINFO_H
#define DISASSEMBLY_VIEWER_TREEINFO_H

#include <QWidget>
#include <elf.h>


QT_BEGIN_NAMESPACE
namespace Ui {
    class treeinfo;
}

QT_END_NAMESPACE

class treeinfo : public QWidget {
    Q_OBJECT

public:
    explicit treeinfo(QWidget *parent = nullptr);

    ~treeinfo() override;

    void setDisplayInfo(int index);

    void setStringTable(const QHash<QString, QVector<QString> > &table);

    void setSymbolTable(const QHash<QString, QList<QPair<QString, Elf64_Sym> > > &table);

private:
    Ui::treeinfo *ui;
    QHash<QString, QVector<QString> > stringTables;

    QVector<QString> stringTableColumn = {"Sections"};

    QHash<QString, QList<QPair<QString, Elf64_Sym> > > symbolTables;
    QVector<QString> symbolTableColumn = {"Sections"};

    void showStringTables();
    void showSymbolTables();
};


#endif //DISASSEMBLY_VIEWER_TREEINFO_H
