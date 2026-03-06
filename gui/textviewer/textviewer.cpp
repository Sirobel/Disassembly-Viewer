//
// Created by finns on 21.02.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_textviewer.h" resolved

#include "textviewer.h"

#include <iostream>

#include "ui_textviewer.h"
#include "x86_64elf.h"
#include <QMessageBox>
#include "Disassembler.h"
#include "x86_64Disasm.h"
#include <chrono>
#include <future>
#include <semaphore>
#include <QDesktopServices>
#include <QToolTip>

textviewer::textviewer(QWidget *parent) : QWidget(parent), ui(new Ui::textviewer) {
    ui->setupUi(this);
}

void textviewer::openFile(const QString &filePath) {
    using clock = std::chrono::high_resolution_clock;
    if (filePath.isEmpty()) {
        return;
    }
    ui->textBrowser->clear();

    std::cout << "Open File: " << filePath.toStdString() << std::endl;
    try {
        auto start = std::chrono::high_resolution_clock::now();
        auto elepasedTime = 0.0;

        elf = std::make_unique<x86_64elf>(filePath.toStdString());
        x86_64Disasm disasm(*elf);
        Disassembler &disassembler = disasm;

        QString text;

        std::vector<std::string> sections = {".init", ".plt", ".plt.got", ".plt.sec", ".text", ".fini"};
        //sections = {".plt.got"};
        std::vector<std::future<std::string> > futures;
        std::counting_semaphore<4> sem(4);

        std::chrono::duration<double, std::milli> ms = clock::now() - start;
        start = std::chrono::high_resolution_clock::now();
        std::cout << "Ready to start futures" << ": " << ms.count() << " ms\n";
        elepasedTime += ms.count();

        for (const std::string &sec: sections) {
            sem.acquire();
            futures.push_back(std::async(std::launch::async, [&sem, this, &disassembler, sec] {
                struct Releaser {
                    std::counting_semaphore<4> &s;

                    explicit Releaser(std::counting_semaphore<4> &sem) : s(sem) {
                    }

                    ~Releaser() { s.release(); }
                } releaser(sem);

                auto data = this->elf->getSection(sec);
                auto ret = disassembler.disassemblePart(data, this->elf->getAddressOfSegment(sec));
                return ret;
            }));
        }

        int index = 0;
        for (auto &f: futures) {
            text += "Disassembly of section " + sections[index++] + "\n";
            text += QString::fromStdString(f.get()) + "\n";
        }

        ms = clock::now() - start;
        start = std::chrono::high_resolution_clock::now();
        std::cout << "futures complete " << ": " << ms.count() << " ms\n";
        elepasedTime += ms.count();


        ms = clock::now() - start;
        start = std::chrono::high_resolution_clock::now();
        std::cout << "ready to set HTML " << ": " << ms.count() << " ms\n";
        elepasedTime += ms.count();


        ui->textBrowser->setHtml(text);

        ms = clock::now() - start;
        std::cout << "completed to set HTML " << ": " << ms.count() << " ms\n";

        std::cout << "complete time :" << elepasedTime + ms.count() << std::endl;
    } catch (std::runtime_error &e) {
        QMessageBox::critical(this, tr("Error"), e.what());
    }
}

textviewer::~textviewer() {
    delete ui;
}

void textviewer::on_textBrowser_anchorClicked(const QUrl &arg1) {
    if (!arg1.isRelative()) {
        QDesktopServices::openUrl(arg1);
        return;
    }

    auto pos = arg1.path();
    std::cout << "clicked " << pos.toStdString() << std::endl;
    QTextCursor cursor = ui->textBrowser->document()->find(pos + ":");

    if (!cursor.isNull()) {
        std::cout << "jumped to" << pos.toStdString() << std::endl;
        ui->textBrowser->setTextCursor(cursor);
        ui->textBrowser->ensureCursorVisible();
        return;
    }

    QToolTip::showText(QCursor::pos(), "Link target not in Textview");
}
