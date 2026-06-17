//
// Created by finns on 08.04.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_fileinfo.h" resolved

#include "fileinfo.h"

#include <iostream>
#include <chrono>

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
    using clock = std::chrono::high_resolution_clock;
    auto sectionTime = std::chrono::high_resolution_clock::now();
    std::cout << "start FileInfoDisplay" << std::endl;

    ui->commandLabel->setText(commands.value(index, "unknown"));

    if (index == 0 || index == 1) {
        columInfo->setSection(sections);
        columInfo->setElfHeader(elfHeader);

        columInfo->setDisplayInfo(index);
        ui->stackedWidget->setCurrentWidget(columInfo);
    }
    if (index == 2 || index == 3 || index == 4 || index == 5) {
        treeInfo->setStringTable(stringTables);
        treeInfo->setSymbolTable(symbol64Tables);
        treeInfo->setRelocationTable(relocation64Tables);
        treeInfo->setProgramHeader(programHeaders);
        treeInfo->setSections(sections);

        treeInfo->setDisplayInfo(index);
        ui->stackedWidget->setCurrentWidget(treeInfo);
    }

    std::chrono::duration<double, std::milli> duration = clock::now() - sectionTime;
    std::cout << "finished FileInfoDisplay in " << duration.count() << std::endl;
}

void fileinfo::setSectionNames(const std::vector<std::pair<std::string, Elf64_Shdr> > &data) {
    sections.clear();

    for (const auto &[name,header]: data) {
        sections.emplace_back(QString::fromStdString(name), header);
    }
}

void fileinfo::setStringTables(const std::vector<std::pair<std::string, std::vector<std::string> > > &tables) {
    stringTables.clear();

    for (const auto &[section,data]: tables) {
        QVector<QString> table;
        for (auto &s:data) {
            table.append(QString::fromStdString(s));
        }
        stringTables[QString::fromStdString(section)].append(table);
    }
}

void fileinfo::setSymbolTables(const std::vector<std::pair<std::string, std::pair<std::string, Elf64_Sym> > > &tables) {
    symbol64Tables.clear();

    for (const auto &[section,data]: tables) {
        symbol64Tables[QString::fromStdString(section)].emplace_back(QString::fromStdString(data.first), data.second);
    }
}

void fileinfo::setElfHeader(const Elf64_Ehdr &header) {
    elfHeader = header;
}

void fileinfo::setRelocations(
    const std::unordered_map<std::string, std::pair<std::string, std::vector<std::pair<std::string, Elf64_Rela> > > > &
    tables) {
    relocation64Tables.clear();

    for (const auto &[section,data]: tables) {
        QList<QPair<QString, Elf64_Rela> > relaList;
        for (const auto &[name,rela]: data.second) {
            ///relocation64Tables[QString::fromStdString(section)].emplace_back(QString::fromStdString(name), rela);
            relaList.emplace_back(QString::fromStdString(name), rela);
        }
        relocation64Tables[QString::fromStdString(section)].second.append(relaList);
        relocation64Tables[QString::fromStdString(section)].first = QString::fromStdString(data.first);
    }
}

void fileinfo::setProgramHeaders(const std::vector<Elf64_Phdr> &data) {
    programHeaders.clear();
    programHeaders = QVector<Elf64_Phdr>(data.begin(), data.end());
}
