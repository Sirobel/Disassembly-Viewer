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
        //sections = {".plt.got"};
        std::vector<std::future<std::string> > futures;
        std::counting_semaphore<4> sem(4);
        std::cout << elf->lookupRangeSymbol(0xa000) << std::endl;

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
