//
// Created by finns on 08.04.26.
//

#ifndef DISASSEMBLY_VIEWER_FILEINFO_H
#define DISASSEMBLY_VIEWER_FILEINFO_H

#include <QWidget>
#include <QHash>
#include <textviewer/infos/columinfo.h>

#include "textviewer/infos/treeinfo.h"


QT_BEGIN_NAMESPACE
namespace Ui {
    class fileinfo;
}

QT_END_NAMESPACE

class fileinfo : public QWidget {
    Q_OBJECT

public:
    explicit fileinfo(QWidget *parent = nullptr);

    ~fileinfo() override;

    void changeWidget(int index);

    void setSectionNames(const std::vector<std::pair<std::string, Elf64_Shdr> > &data);

    void setStringTables(const std::vector<std::pair<std::string, std::vector<char> > > &tables);

    void setSymbolTables(const std::vector<std::pair<std::string, std::pair<std::string, Elf64_Sym> > > &tables);

    void setElfHeader(const Elf64_Ehdr &header);

    void setRelocations(
        const std::unordered_map<std::string, std::pair<std::string, std::vector<std::pair<std::string,
            Elf64_Rela> > > > &tables);
    void setProgramHeaders(const std::vector<Elf64_Phdr> &data);

private:
    Ui::fileinfo *ui;
    columinfo *columInfo;
    treeinfo *treeInfo;
    Elf64_Ehdr elfHeader{};

    QVector<QPair<QString, Elf64_Shdr> > sections;
    QVector<Elf64_Phdr> programHeaders;
    QHash<QString, QVector<QString> > stringTables;
    QHash<QString, QList<QPair<QString, Elf64_Sym> > > symbol64Tables;
    QHash<QString, QPair<QString, QList<QPair<QString, Elf64_Rela> > > > relocation64Tables;

    QVector<QString> commands = {
        "readelf -h <file>", "readelf -S <file>", "objdump -j <stringTable> <file>",
        "objdump -j <symbolTable> <file>", "objdump -r <file>","readelf -l <file>"
    };
};


#endif //DISASSEMBLY_VIEWER_FILEINFO_H
