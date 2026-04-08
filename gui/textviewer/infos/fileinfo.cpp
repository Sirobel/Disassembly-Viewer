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

    ui->stackedWidget->addWidget(columInfo);
}

fileinfo::~fileinfo() {
    delete ui;
}

void fileinfo::changeWidget(const int index) {
    if (index == 0 || index == 1) {
        columInfo->setSectionNames(sectionNames);
        ui->stackedWidget->setCurrentWidget(columInfo);
        columInfo->setDisplayInfo(index);
    }
}

void fileinfo::setSectionNames(const std::vector<std::string> &names) {
    sectionNames.clear();

    for (const auto &name: names) {
        sectionNames.append(QString::fromStdString(name));
    }
}
