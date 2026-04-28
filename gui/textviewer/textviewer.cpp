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
#include <qstyle.h>
#include <QToolTip>
#include <QKeySequence>
#include <QShortcut>
#include <QInputDialog>
#include <QMouseEvent>
#include <qregularexpression.h>
#include "textviewer/DisasmHighlighter.h"


textviewer::textviewer(QWidget *parent) : QWidget(parent), ui(new Ui::textviewer),
                                          settings("Sirobel", "Disassembly-Viewer") {
    ui->setupUi(this);
    refresh();
    ui->searchGroupBox->hide();


    shortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
    connect(shortcut, &QShortcut::activated, this, &textviewer::toggleSearchbar);

    fileInfo = new fileinfo(this);
    fileInfo->close();

    new DisasmHighlighter(ui->textBrowser->document());
}

void textviewer::openFile(const QString &filePath) {
    ui->searchGroupBox->hide();

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

        ui->textBrowser->setPlainText(text);

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

        fileInfo->setSectionNames(elf->getSectionNames());
        fileInfo->setStringTables(elf->getStringTables());
        fileInfo->setSymbolTables(elf->getSymbolTablesElf64());
        fileInfo->setElfHeader(elf->getElf64Header());
        fileInfo->setRelocations(elf->getRelaTables64());

        generateAddressLookup();

        const std::chrono::duration<double, std::milli> ms = clock::now() - start;
        elapsedTime = ms.count();

        std::cout << "complete time :" << elapsedTime << std::endl;
    } catch (std::runtime_error &e) {
        QMessageBox::critical(this, tr("Error"), e.what());
    }
}

void textviewer::refresh() {
    const QString css = QString(R"(
p {
    font-size: %1px;
    font-family: monospace;
    padding: 6px;
    color: %3;
}
 a {
    color: %2;
    font-size: %1;
    text-decoration: %4;
}
)")
            .arg(settings.value("fontSize", 16).toInt())
            .arg(settings.value("linkColor", "#0000EE").toString())
            .arg(settings.value("textColor", "#000000").toString())
            .arg(settings.value("linkUnderscore", Qt::Unchecked).toInt() == Qt::Checked ? "underline" : "none");


    ui->textBrowser->document()->setDefaultStyleSheet(css);
    ui->memBar->refresh();
    ui->textBrowser->setMouseTracking(true);
    ui->textBrowser->viewport()->setMouseTracking(true);
    ui->textBrowser->viewport()->installEventFilter(this);
}

void textviewer::showFileInfo(const int index) {
    fileInfo->setWindowFlag(Qt::Window);
    fileInfo->setWindowModality(Qt::ApplicationModal);
    fileInfo->show();
    fileInfo->raise();
    fileInfo->activateWindow();
    fileInfo->changeWidget(index);
}

textviewer::~textviewer() {
    delete ui;
}

void textviewer::toggleSearchbar() {
    if (ui->searchGroupBox->isHidden()) {
        ui->searchGroupBox->show();
        ui->searchLineEdit->setFocus();
    } else
        ui->searchGroupBox->hide();
}

int textviewer::countSearchResults(const QString &text) {
    if (text.isEmpty())
        return 0;

    QTextDocument *doc = ui->textBrowser->document();
    QTextCursor cursor(doc);
    int count = 0;

    while (!cursor.isNull() && !cursor.atEnd()) {
        cursor = doc->find(text, cursor);
        if (!cursor.isNull())
            count++;
    }

    return count;
}

int textviewer::findCurrentMatch(const QString &text) {
    if (text.isEmpty())
        return 0;

    int count = 0;

    QTextDocument *doc = ui->textBrowser->document();
    QTextCursor cursor(doc);

    while (!cursor.isNull() && !cursor.atEnd()) {
        cursor = doc->find(text, cursor);
        if (cursor.isNull())
            return 0;
        count++;

        if (cursor.selectionStart() == ui->textBrowser->textCursor().selectionStart())
            break;
    }
    return count;
}

void textviewer::updateSearchLabel() {
    if (totalMatches == 0)
        ui->searchMatchesLabel->setText("");
    else
        ui->searchMatchesLabel->setText(
            QString("%1 / %2").arg(currentMatch).arg(totalMatches)
        );
}

void textviewer::jumpToTarget(const QString &target) {
    if (!target.startsWith("0x")) {
        QDesktopServices::openUrl("https://www.felixcloutier.com/x86/" + target);
        return;
    }


    if (const auto address = addressLookup.find(target); address != addressLookup.end()) {
        std::cout << "jumped to" << target.toStdString() << std::endl;
        QTextCursor cursor(ui->textBrowser->document()->findBlockByNumber(address.value()));
        cursor.select(QTextCursor::LineUnderCursor);
        ui->textBrowser->setTextCursor(cursor);
        ui->textBrowser->ensureCursorVisible();

        return;
    }

    QToolTip::showText(QCursor::pos(), "Link target not in Textview");
}

void textviewer::generateAddressLookup() {
    addressLookup.clear();

    const QTextDocument *doc = ui->textBrowser->document();

    for (QTextBlock block = doc->begin(); block != doc->end(); block = block.next()) {
        if (const QString &text = block.text(); text.size() > 2 && text.startsWith("\t0x")) {
            if (const long long endPos = text.indexOf(':'); endPos > 0) {
                QString addr = text.left(endPos);
                addr.remove("\t");
                addressLookup[addr] = block.blockNumber();
            }
        }
    }
}

bool textviewer::eventFilter(QObject *obj, QEvent *event) {
    if (obj != ui->textBrowser->viewport() || event->type() != QEvent::MouseButtonRelease)
        return QWidget::eventFilter(obj, event);
    const auto *me = dynamic_cast<QMouseEvent *>(event);
    QTextCursor cursor = ui->textBrowser->cursorForPosition(me->pos());
    QTextCursor cursor2 = ui->textBrowser->cursorForPosition(me->pos());


    // Get the clicked position within the line
    int clickedCol = cursor.positionInBlock();

    // Select the entire line
    cursor.select(QTextCursor::LineUnderCursor);
    cursor2.select(QTextCursor::WordUnderCursor);
    QString line = cursor.selectedText();

    QString line2 = cursor2.selectedText();

    if (line2.startsWith("\u200c") && line2.endsWith("\u200c")) {
        line2.remove("\u200c");
        std::cout << "clicked " << line2.toStdString() << std::endl;
        jumpToTarget(line2);
        return true;
    }

    // Find "target 0x..." pattern and check if click was on it
    static const QRegularExpression targetRx(R"(#target\s+(0x[0-9a-fA-F]+)(?:\s+<(.+)>)?)");
    QRegularExpressionMatchIterator it = targetRx.globalMatch(line);

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        const long long start = match.capturedStart(0);
        if (const long long end = match.capturedEnd(0); clickedCol >= start && clickedCol < end) {
            const QString address = match.captured(1);
            const QString symbol = match.captured(2); // may be empty
            qDebug() << "Clicked:" << address << symbol;
            jumpToTarget(address);
            return true;
        }
    }

    return QWidget::eventFilter(obj, event);
}


void textviewer::on_searchLineEdit_textChanged(const QString &text) {
    ui->textBrowser->find(text);
    totalMatches = countSearchResults(text);
    currentMatch = findCurrentMatch(text);

    updateSearchLabel();
}

void textviewer::on_previousSearchPushButton_clicked() {
    if (ui->searchLineEdit->text().isEmpty())
        return;


    if (!ui->textBrowser->find(ui->searchLineEdit->text(), QTextDocument::FindBackward)) {
        ui->textBrowser->moveCursor(QTextCursor::End);
        ui->textBrowser->find(ui->searchLineEdit->text(), QTextDocument::FindBackward);
    }
    currentMatch--;
    if (currentMatch < 1)
        currentMatch = totalMatches;

    updateSearchLabel();
}


void textviewer::on_nextSearchPushButton_clicked() {
    if (ui->searchLineEdit->text().isEmpty())
        return;


    if (!ui->textBrowser->find(ui->searchLineEdit->text())) {
        ui->textBrowser->moveCursor(QTextCursor::Start);
        ui->textBrowser->find(ui->searchLineEdit->text());
    }
    currentMatch++;
    if (currentMatch > totalMatches)
        currentMatch = 1;


    updateSearchLabel();
}
