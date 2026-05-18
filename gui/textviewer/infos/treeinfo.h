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

    void setRelocationTable(const QHash<QString, QPair<QString, QList<QPair<QString, Elf64_Rela> > > > &tables);

    void setProgramHeader(const QVector<Elf64_Phdr> &data);

    void setSections(const QVector<QPair<QString, Elf64_Shdr> > &data);

private:
    Ui::treeinfo *ui;
    QHash<QString, QVector<QString> > stringTables;
    QVector<QString> stringTableColumn = {"Sections"};

    QHash<QString, QList<QPair<QString, Elf64_Sym> > > symbolTables;
    QVector<QString> symbolTableColumn = {"Sections", "Type", "Binding", "Visibility"};

    QHash<QString, QPair<QString, QList<QPair<QString, Elf64_Rela> > > > relocationTables;
    QVector<QString> relocationTableColumn = {"Section", "Address", "Addend", "Type", "Symbol index"};

    QVector<QString> programHeaderColumn = {
        "Name/Type", "Flags", "Offset", "Virtual address", "Physical address", "File size", "Memory size", "Alignment"
    };
    QVector<QPair<QString, Elf64_Shdr> > sections;
    QVector<Elf64_Phdr> programHeaders;

    void showStringTables();

    void showSymbolTables();

    void showRelocationTables();

    void showProgramTable();
};


#endif //DISASSEMBLY_VIEWER_TREEINFO_H
