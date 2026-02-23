//
// Created by finns on 21.02.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_textviewer.h" resolved

#include "textviewer.h"

#include <iostream>

#include "ui_textviewer.h"
#include "x86_64elf.h"
#include <QMessageBox>


textviewer::textviewer(QWidget *parent) : QWidget(parent), ui(new Ui::textviewer) {
    ui->setupUi(this);
}

void textviewer::openFile(const QString &filePath) {
    if (filePath.isEmpty()) {
        return;
    }
    ui->textBrowser->clear();

    std::cout << "Open File: " << filePath.toStdString() << std::endl;
    try {
        elf = std::make_unique<x86_64elf>(filePath.toStdString());

        std::vector<uint8_t> data = elf->getSection(".text");
        QByteArray arr(reinterpret_cast<const char *>(data.data()), data.size());
        QString s = arr.toHex(' ');
        ui->textBrowser->setText(s);
    } catch (std::runtime_error &e) {
        QMessageBox::critical(this, tr("Error"), e.what());
    }
}

textviewer::~textviewer() {
    delete ui;
}
