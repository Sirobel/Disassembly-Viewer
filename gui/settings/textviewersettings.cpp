//
// Created by finns on 16.03.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_textviewersettings.h" resolved

#include "textviewersettings.h"
#include "ui_textviewersettings.h"


textviewersettings::textviewersettings(QWidget *parent) : QWidget(parent), ui(new Ui::textviewersettings) {
    ui->setupUi(this);
}

textviewersettings::~textviewersettings() {
    delete ui;
}