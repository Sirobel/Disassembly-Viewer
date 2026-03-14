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
    ui->memBar->setMinimumSize(300, 20);
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
        auto elapsedTime = 0.0;

        auto sectionTime = std::chrono::high_resolution_clock::now();
        std::cout << "start disassembly" << std::endl;

        elf = std::make_unique<x86_64elf>(filePath.toStdString());
        x86_64Disasm disasm(*elf);
        Disassembler &disassembler = disasm;

        QString text;

        std::vector<std::string> sections = {".init", ".plt", ".plt.got", ".plt.sec", ".text", ".fini"};
        //sections = {".plt.got"};
        std::vector<std::future<std::string> > futures;
        std::counting_semaphore<4> sem(4);


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

        std::chrono::duration<double, std::milli> duration = clock::now() - sectionTime;
        std::cout << "finished disassembly in " << duration.count() << std::endl
                << "start setHTML" << std::endl;
        sectionTime = std::chrono::high_resolution_clock::now();

        ui->textBrowser->setHtml(text);

        duration = clock::now() - sectionTime;
        std::cout << "finished setHTML in " << duration.count() << std::endl
                << "start memBar" << std::endl;
        sectionTime = std::chrono::high_resolution_clock::now();


        auto progHeaders = elf->getLoadableProgramHeaders();
        auto secHeaders = elf->getLoadableSectionHeaders();

        std::vector<membar::Segment> programBar;
        std::vector<std::vector<membar::Segment> > sectionBar;
        sectionBar.resize(progHeaders.size());
        auto table = elf->getSectionHeadersNames();

        for (int i = 0; i < progHeaders.size(); ++i) {
            programBar.emplace_back("", progHeaders[i].first, progHeaders[i].second);

            for (auto &sec: secHeaders) {
                if (sec.first >= progHeaders[i].first && sec.first + sec.second <= progHeaders[i].first + progHeaders[i]
                    .second) {
                    for (auto &[fst, snd]: table) {
                        if (fst == sec.first) {
                            sectionBar[i].emplace_back(QString::fromStdString(snd), sec.first, sec.second);
                            break;
                        }
                    }
                }
            }
        }

        ui->memBar->initializeBar(sectionBar, programBar);

        duration = clock::now() - sectionTime;
        std::cout << "finished memBar in " << duration.count() << std::endl;
        sectionTime = std::chrono::high_resolution_clock::now();

        const std::chrono::duration<double, std::milli> ms = clock::now() - start;
        elapsedTime = ms.count();

        std::cout << "complete time :" << elapsedTime << std::endl;
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
