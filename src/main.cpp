//
// Created by finns on 20.02.26.
//
#include <QApplication>
#include <QPushButton>
#include "mainwindow/windowhandler.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    windowhandler app;
    app.show();
    return QApplication::exec();
}