//
// Created by finns on 20.02.26.
//
#include <QApplication>
#include <QPushButton>
#include <qscreen.h>

#include "mainwindow/windowhandler.h"

int main(int argc, char *argv[]) {
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QApplication a(argc, argv);


    windowhandler app;
    app.show();
    return QApplication::exec();
}