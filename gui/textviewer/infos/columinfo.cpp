//
// Created by finns on 08.04.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_columinfo.h" resolved

#include "columinfo.h"
#include "ui_columinfo.h"


columinfo::columinfo(QWidget *parent) : QWidget(parent), ui(new Ui::columinfo) {
    ui->setupUi(this);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void columinfo::setDisplayInfo(const int index) {
    ui->tableWidget->clear();

    switch (index) {
        case 0:
            showHeaderInfo();
            break;
        case 1:
            showSectionInfo();
            break;
        default:
            break;
    }
}

void columinfo::setSectionNames(const QVector<QString> &names) {
    sectionHeaderRow.clear();
    sectionHeaderRow = names;
}

void columinfo::setElfHeader(const Elf64_Ehdr &header) {
    fileHeader = header;
}

void columinfo::showHeaderInfo() {
    ui->tableWidget->setColumnCount(static_cast<int>(fileHeaderColum.size()));
    ui->tableWidget->setRowCount(static_cast<int>(fileHeaderRow.size()));

    ui->tableWidget->setHorizontalHeaderLabels(fileHeaderColum);

    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(fileHeaderRow[i]));
    }
    update();
}

void columinfo::showSectionInfo() {
    ui->tableWidget->setColumnCount(static_cast<int>(sectionHeaderColum.size()));
    ui->tableWidget->setRowCount(static_cast<int>(sectionHeaderRow.size()));
    ui->tableWidget->setHorizontalHeaderLabels(sectionHeaderColum);

    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(sectionHeaderRow[i]));
    }
    update();
}

columinfo::~columinfo() {
    delete ui;
}
