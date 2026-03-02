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
        const auto start = std::chrono::high_resolution_clock::now();
        elf = std::make_unique<x86_64elf>(filePath.toStdString());
        x86_64Disasm disasm(*elf);
        Disassembler &disassembler = disasm;

        QString text;

        std::vector<std::string> sections = {".init", ".plt", ".plt.got", ".plt.sec", ".text", ".fini"};
        std::vector<std::future<std::string> > futures;
        std::counting_semaphore<4> sem(4);

        for (const std::string &sec: sections) {
            sem.acquire();
            futures.push_back(std::async(std::launch::async, [&sem, this, &disassembler, sec] {
                auto data = elf->getSection(sec);
                auto ret = disassembler.disassemblePart(data, elf->getAddressOfSegment(sec));
                sem.release();
                return ret;
            }));
        }

        for (auto &f: futures) {
            text += QString::fromStdString(f.get());
        }


        ui->textBrowser->setText(text);
        const std::chrono::duration<double, std::milli> ms = clock::now() - start;
        std::cout << "Elapsed time to disassemble " << ": " << ms.count() << " ms\n";
    } catch (std::runtime_error &e) {
        QMessageBox::critical(this, tr("Error"), e.what());
    }
}

textviewer::~textviewer() {
    delete ui;
}
