//
// Created by finns on 21.02.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_textviewer.h" resolved

#include "textviewer.h"

#include <iostream>

#include "ui_textviewer.h"
#include "x86_64elf.h"
#include <QMessageBox>
#include "x86_64Disasm.h"
#include <chrono>
#include <future>
#include <QDesktopServices>
#include <QToolTip>
#include <QKeySequence>
#include <QShortcut>
#include <QInputDialog>
#include <QMouseEvent>
#include <qregularexpression.h>

#include "tree/DisasmDelegate.h"
#include "textviewer/tree/DisasmModel.h"


textviewer::textviewer(QWidget *parent) : QWidget(parent), ui(new Ui::textviewer),
                                          settings("Sirobel", "Disassembly-Viewer") {
    ui->setupUi(this);
    ui->searchGroupBox->hide();
    model = new DisasmModel(this);
    ui->treeView->setModel(model);
    delegate = new DisasmDelegate(ui->treeView);
    ui->treeView->setItemDelegate(delegate);
    ui->treeView->setRootIsDecorated(true);
    ui->treeView->viewport()->installEventFilter(this);
    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->treeView->setToolTip("");
    ui->treeView->setMouseTracking(true);
    ui->treeView->setUniformRowHeights(true);


    shortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
    connect(shortcut, &QShortcut::activated, this, &textviewer::toggleSearchbar);

    fileInfo = new fileinfo(this);
    fileInfo->close();


    searchTimer = new QTimer(this);
    searchTimer->setSingleShot(true);
    connect(searchTimer, &QTimer::timeout, this, &textviewer::search);

    refresh();
}

void textviewer::openFile(const QString &filePath) {
    ui->searchGroupBox->hide();
    ui->treeView->reset();
    auto sections = settings.value("sections").toStringList();


    using clock = std::chrono::high_resolution_clock;
    if (filePath.isEmpty()) {
        return;
    }

    std::cout << "Open File: " << filePath.toStdString() << std::endl;
    try {
        const auto start = std::chrono::high_resolution_clock::now();
        auto elapsedTime = 0.0;

        auto sectionTime = std::chrono::high_resolution_clock::now();
        std::cout << "start disassembly" << std::endl;

        elf = std::make_unique<x86_64elf>(filePath.toStdString());


        QVector<DisasmModel::Section> section;
        const x86_64Disasm disasm(*elf);

        std::ranges::sort(sections,[this](const QString &a, const QString &b) {
            return elf->getAddressOfSegment(a.toStdString()) < elf->getAddressOfSegment(b.toStdString());
        });

        // Step 1: read all data under the mutex (sequential, fast)
        struct SectionRaw { QString displayName; std::vector<uint8_t> data; uint64_t addr; };
        std::vector<SectionRaw> raw;
        raw.reserve(sections.size());
        for (const auto &sec : sections) {
            raw.push_back({
                "Disassembly of Section " + sec,
                elf->getSection(sec.toStdString()),
                elf->getAddressOfSegment(sec.toStdString())
            });
        }

        section.resize(static_cast<long>(raw.size()));
        std::vector<std::future<void>> futs;
        futs.reserve(raw.size());
        for (int i = 0; i < static_cast<int>(raw.size()); ++i) {
            futs.push_back(std::async(std::launch::async, [&, i] {
                section[i] = {
                    raw[i].displayName,
                    disasm.disassemblePartToSections(raw[i].data, raw[i].addr)
                };
            }));
        }
        for (auto &f : futs) f.get();

        std::chrono::duration<double, std::milli> duration = clock::now() - sectionTime;
        std::cout << "finished disassembly in " << duration.count() << std::endl
                << "start SetText" << std::endl;
        sectionTime = std::chrono::high_resolution_clock::now();

        if (!sections.empty()) {
            ui->treeView->setUpdatesEnabled(false);
            model->setSections(std::move(section));
            ui->treeView->expandToDepth(1);

            for (int s = 0; s < model->rowCount({}); ++s) {
                ui->treeView->setFirstColumnSpanned(s, QModelIndex(), true);
                QModelIndex secIdx = model->index(s, 0, {});
                for (int f = 0; f < model->rowCount(secIdx); ++f) {
                    ui->treeView->setFirstColumnSpanned(f, secIdx, true);
                }
            }
            ui->treeView->setUpdatesEnabled(true);

        } else
            ui->treeView->reset();


        duration = clock::now() - sectionTime;
        std::cout << "finished setText in " << duration.count() << std::endl
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
        std::cout << "finished memBar in " << duration.count() << std::endl <<"start  FileInfo" << std::endl;
        sectionTime = std::chrono::high_resolution_clock::now();


        fileInfo->setSectionNames(elf->getSections64());
        fileInfo->setStringTables(elf->getStringTables());
        fileInfo->setSymbolTables(elf->getSymbolTablesElf64());
        fileInfo->setElfHeader(elf->getElf64Header());
        fileInfo->setRelocations(elf->getRelaTables64());
        fileInfo->setProgramHeaders(elf->getProgramHeaders64());

        duration = clock::now() - sectionTime;
        std::cout << "finished FileInfo in " << duration.count() << std::endl;


        const std::chrono::duration<double, std::milli> ms = clock::now() - start;
        elapsedTime = ms.count();

        std::cout << "complete time :" << elapsedTime << std::endl;
    } catch (std::runtime_error &e) {
        QMessageBox::critical(this, tr("Error"), e.what());
    }
}

void textviewer::refresh() {
    using clock = std::chrono::high_resolution_clock;
    auto sectionTime = std::chrono::high_resolution_clock::now();
    std::cout << "textviewer refresh starts" << std::endl;
    ui->memBar->refresh();

    QFont font = ui->treeView->font();
    font.setPointSize(settings.value("fontSize", 16).toInt());
    ui->treeView->setFont(font);
    delegate->refresh();
    const std::chrono::duration<double, std::milli> duration = clock::now() - sectionTime;
    std::cout << "textviewer refresh end with" <<duration<<"ms"<< std::endl;
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


void textviewer::updateSearchLabel() {
    if (totalMatches == 0)
        ui->searchMatchesLabel->setText("");
    else
        ui->searchMatchesLabel->setText(
            QString("%1 / %2").arg(currentMatch).arg(totalMatches)
        );
}

void textviewer::search() {
    const QString text = ui->searchLineEdit->text();
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    if (text.isEmpty()) {
        currentSearchIndex = {};
        totalMatches = 0;
        currentMatch = 0;
        updateSearchLabel();
        return;
    }


    totalMatches = model->countMatches(text);
    currentSearchIndex = model->findNext(text, {});
    currentMatch = 1;

    if (currentSearchIndex.isValid()) {
        ui->treeView->scrollTo(currentSearchIndex, QAbstractItemView::PositionAtCenter);
        ui->treeView->selectionModel()->setCurrentIndex(currentSearchIndex,
                                                        QItemSelectionModel::ClearAndSelect |
                                                        QItemSelectionModel::Rows);
    }

    const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() <<
            "[ms]" << std::endl;

    updateSearchLabel();
}

void textviewer::jumpToTarget(const QString &target) {
    std::cout << "Jumping to " << target.toStdString() << std::endl;

    if (!target.startsWith("0x")) {
        QDesktopServices::openUrl("https://www.felixcloutier.com/x86/" + target);
        return;
    }

    const auto index = model->findAddress(target);
    if (!index.isValid()) {
        QTimer::singleShot(100, this, []() {
            QToolTip::showText(QCursor::pos(), "Link target not in Textview");
        });
        return;
    }

    ui->treeView->scrollTo(index, QAbstractItemView::PositionAtTop);
    ui->treeView->selectionModel()->setCurrentIndex(
        index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void textviewer::handleLink(const QString &link) {
    if (link.isEmpty())
        return;

    static const QRegularExpression targetRx(R"(#target\s+(0x[0-9a-fA-F]+)(?:\s+<(.+)>)?)");
    QRegularExpressionMatch match = targetRx.match(link);

    if (match.hasMatch()) {
        jumpToTarget(match.captured(1));
    } else {
        jumpToTarget(link);
    }
}

bool textviewer::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->treeView->viewport() && event->type() == QEvent::MouseButtonRelease) {
        const auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            const auto index = ui->treeView->indexAt(mouseEvent->pos());
            if (index.isValid() && index.parent().isValid() && (index.column() == 2 || index.column() == 4) && !index.
                data().toString().isEmpty()) {
                handleLink(index.data().toString());
                return true;
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}


void textviewer::on_searchLineEdit_textChanged(const QString &text) {
    searchTimer->stop();
    searchTimer->start(150);
}

void textviewer::on_previousSearchPushButton_clicked() {
    const QString text = ui->searchLineEdit->text();
    if (text.isEmpty()) return;

    QModelIndex prev = model->findPrev(text, currentSearchIndex);
    if (!prev.isValid()) {
        prev = model->findPrev(text, {});
        currentMatch = totalMatches;
    } else {
        currentMatch--;
    }

    currentSearchIndex = prev;
    if (currentSearchIndex.isValid()) {
        ui->treeView->scrollTo(currentSearchIndex, QAbstractItemView::PositionAtCenter);
        ui->treeView->selectionModel()->setCurrentIndex(
            currentSearchIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    }
    updateSearchLabel();
}

void textviewer::on_nextSearchPushButton_clicked() {
    const QString text = ui->searchLineEdit->text();
    if (text.isEmpty()) return;

    QModelIndex next = model->findNext(text, currentSearchIndex);
    if (!next.isValid()) {
        next = model->findNext(text, {});
        currentMatch = 1;
    } else {
        currentMatch++;
    }

    currentSearchIndex = next;
    if (currentSearchIndex.isValid()) {
        ui->treeView->scrollTo(currentSearchIndex, QAbstractItemView::PositionAtCenter);
        ui->treeView->selectionModel()->setCurrentIndex(
            currentSearchIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    }
    updateSearchLabel();
}

void textviewer::on_closeSearchButton_clicked() {
    ui->searchGroupBox->hide();
}
