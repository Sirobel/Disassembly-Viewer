//
// Created by finns on 08.04.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_fileinfo.h" resolved

#include "fileinfo.h"

#include "ui_fileinfo.h"
#include "columinfo.h"


fileinfo::fileinfo(QWidget *parent) : QWidget(parent), ui(new Ui::fileinfo) {
    ui->setupUi(this);
    columInfo = new columinfo(this);
    treeInfo = new treeinfo(this);

    ui->stackedWidget->addWidget(columInfo);
    ui->stackedWidget->addWidget(treeInfo);
}

fileinfo::~fileinfo() {
    delete ui;
}

void fileinfo::changeWidget(const int index) {
    if (index == 0 || index == 1) {
        columInfo->setSectionNames(sectionNames);
        columInfo->setDisplayInfo(index);
        ui->stackedWidget->setCurrentWidget(columInfo);
    }
    if (index == 2 || index == 3) {
        treeInfo->setStringTable(stringTables);
        treeInfo->setSymbolTable(symbolTables);
        treeInfo->setDisplayInfo(index);
        ui->stackedWidget->setCurrentWidget(treeInfo);
    }
}

void fileinfo::setSectionNames(const std::vector<std::string> &names) {
    sectionNames.clear();

    for (const auto &name: names) {
        sectionNames.append(QString::fromStdString(name));
    }
}

void fileinfo::setStringTables(const std::vector<std::pair<std::string, std::vector<char> > > &tables) {
    stringTables.clear();

    for (const auto &[section,data]: tables) {
        QVector<QString> table;

        auto it = data.begin();
        while (it != data.end()) {
            auto end = std::find(it, data.end(), '\0');
            table.emplace_back(QString::fromStdString(std::string(it, end)));
            if (end == data.end()) {
                break;
            }
            it = end + 1;
        }

        stringTables[QString::fromStdString(section)].append(table);
    }
}

void fileinfo::setSymbolTables(const std::vector<std::pair<std::string, std::pair<std::string, Elf64_Sym> > > &tables) {
    symbolTables.clear();

    for (const auto &[section,data]: tables) {
        symbolTables[QString::fromStdString(section)].emplace_back(QString::fromStdString(data.first), data.second);
    }
}
