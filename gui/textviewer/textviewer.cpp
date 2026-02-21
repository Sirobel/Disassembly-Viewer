//
// Created by finns on 21.02.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_textviewer.h" resolved

#include "textviewer.h"

#include <iostream>

#include "ui_textviewer.h"


textviewer::textviewer(QWidget *parent) : QWidget(parent), ui(new Ui::textviewer) {
    ui->setupUi(this);

}

void textviewer::openFile(const QString &filePath) {
    std::cout << "Open File: " << filePath.toStdString() << std::endl;
}

textviewer::~textviewer() {
    delete ui;
}
