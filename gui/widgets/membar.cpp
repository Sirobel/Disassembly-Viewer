//
// Created by finns on 10.03.26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_membar.h" resolved

#include "membar.h"

#include <iostream>

#include "ui_membar.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QMouseEvent>
#include <QToolTip>


membar::membar(QWidget *parent) : QWidget(parent), ui(new Ui::membar), settings("Sirobel", "Disassembly-Viewer") {
    ui->setupUi(this);
    drawTarget = -1;
    maxSize = 0;

    hoverTimer.setSingleShot(true);
    hoverTimer.setInterval(1000);
    mousePos = QPoint(0, 0);
    hoverIndex = -1;

    setMouseTracking(true);
    connect(&hoverTimer, &QTimer::timeout, this, [this]() {
        if (hoverIndex == -1)
            return;

        if (drawTarget != -1) {
            QToolTip::showText(mapToGlobal(mousePos),
                               sectionHeaders[drawTarget][hoverIndex].name + " at 0x" + QString::number(
                                   sectionHeaders[drawTarget][hoverIndex].addr, 16), this);
        } else {
            int target = -1;
            for (int i = 0; i < programHeadersRects.size(); ++i) {
                if (programHeadersRects[i].contains(mousePos)) {
                    target = i;
                    break;
                }
            }
            if (target != -1)
                QToolTip::showText(mapToGlobal(mousePos),
                                   sectionHeaders[target][hoverIndex].name + " at 0x" + QString::number(
                                       sectionHeaders[target][hoverIndex].addr, 16), this);
        }
    });
}

membar::~membar() {
    delete ui;
}

void membar::initializeBar(const std::vector<std::vector<Segment> > &secHeader,
                           const std::vector<Segment> &progHeader) {
    maxSize = 0;
    drawTarget = -1;
    hoverIndex = -1;
    hoverTimer.stop();
    programHeaders = progHeader;
    sectionHeaders = secHeader;
    colors.clear();


    colors.resize(static_cast<long>(programHeaders.size()));

    for (int i = 0; i < progHeader.size(); ++i) {
        maxSize += programHeaders[i].size;

        if (colors[i].isValid())
            continue;

        colors[i] = QColor(
            QRandomGenerator::global()->bounded(255),
            QRandomGenerator::global()->bounded(255),
            QRandomGenerator::global()->bounded(255)
        );
    }

    refresh();
}

void membar::refresh() {
    colors.clear();
    drawTarget = -1;
    hoverIndex = -1;
    hoverTimer.stop();

    for (QStringList colorList = settings.value("memBarColors").toStringList(); const auto &c: colorList) {
        colors.push_back(QColor(c));
    }

    colors.resize(static_cast<long>(programHeaders.size()));

    for (int i = 0; i < programHeaders.size(); ++i) {
        if (colors[i].isValid())
            continue;

        colors[i] = QColor(
            QRandomGenerator::global()->bounded(255),
            QRandomGenerator::global()->bounded(255),
            QRandomGenerator::global()->bounded(255)
        );
    }

    fontColor = QColor(settings.value("memBarTextColor").toString());
    if (!fontColor.isValid())
        fontColor = Qt::white;

    borderColor = QColor(settings.value("memBarBorderColor").toString());
    if (!borderColor.isValid())
        borderColor = Qt::black;

    fontSize = settings.value("memBarFontSize").toInt();
    borderSize = settings.value("memBarBorderSize").toInt();

    repaint();
}

void membar::paintEvent(QPaintEvent *event) {
    using clock = std::chrono::high_resolution_clock;
    auto sectionTime = std::chrono::high_resolution_clock::now();
    std::cout << "start MembarPaintEvent" << std::endl;

    if (drawTarget == -1)
        drawAll();
    else
        drawSectionHeaders();

    std::chrono::duration<double, std::milli> duration = clock::now() - sectionTime;
    std::cout << "finished MembarPaintEvent in " << duration.count() << std::endl;
}

void membar::mousePressEvent(QMouseEvent *event) {
    if (drawTarget != -1) {
        drawTarget = -1;
        update();
        std::cout << "new Draw Target: " << drawTarget << std::endl;
        return;
    }


    const QPointF pos = event->pos();

    for (int i = 0; i < programHeadersRects.size(); ++i) {
        if (programHeadersRects[i].contains(pos)) {
            drawTarget = i;
            break;
        }
    }

    update();
    std::cout << "new Draw Target: " << drawTarget << std::endl;
}

void membar::mouseMoveEvent(QMouseEvent *event) {
    const QPointF pos = event->pos();
    int newHover = -1;

    if (drawTarget == -1) {
        for (int i = 0; i < sectionHeadersRects.size(); ++i) {
            if (sectionHeadersRects[i].second.contains(pos)) {
                newHover = sectionHeadersRects[i].first;
                mousePos = event->pos();
                break;
            }
        }
    } else {
        for (int i = 0; i < programHeadersRects.size(); ++i) {
            if (programHeadersRects[i].contains(pos)) {
                newHover = i;
                mousePos = event->pos();
                break;
            }
        }
    }


    if (newHover != hoverIndex) {
        hoverIndex = newHover;
        hoverTimer.stop();

        if (hoverIndex != -1)
            hoverTimer.start();
    }
}

void membar::leaveEvent(QEvent *event) {
    hoverTimer.stop();
    hoverIndex = -1;
}

void membar::drawAll() {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setFont(QFont("Arial", fontSize, QFont::Bold));

    programHeadersRects.clear();
    sectionHeadersRects.clear();


    const double scale = width() / static_cast<double>(maxSize);

    std::vector<double> widths;
    widths.reserve(programHeaders.size());

    for (const auto &header: programHeaders) {
        widths.push_back(static_cast<double>(header.size) * scale);
    }

    double total = 0;
    for (auto &w: widths) {
        if (w < 20)
            w = 20;
        total += w;
    }
    if (total > width()) {
        const double correction = width() / total;
        for (auto &w: widths)
            w *= correction;
    }

    double x = 0;
    for (int i = 0; i < programHeaders.size(); ++i) {
        const auto w = widths[i];
        const double sectionScale = w / static_cast<double>(programHeaders[i].size);

        auto rec = QRectF(x, 0, w, height());
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(rec);

        int count = 0;
        for (const auto &segment: sectionHeaders[i]) {
            painter.setPen(QPen(borderColor, borderSize));
            if (borderSize <= 0)
                painter.setPen(Qt::NoPen);
            auto secRec = QRectF(x + static_cast<double>(segment.addr - programHeaders[i].addr) * sectionScale, 0,
                                 static_cast<double>(segment.size) * sectionScale, height());
            sectionHeadersRects.emplaceBack(count, secRec);

            painter.setBrush(colors[i]);
            painter.drawRect(secRec);

            if (QFontMetrics fm(QFont("Arial", fontSize, QFont::Bold));
                secRec.width() > fm.horizontalAdvance(segment.name)) {
                painter.setPen(fontColor);
                painter.drawText(secRec, Qt::AlignCenter, segment.name);
            }
            count++;
        }

        programHeadersRects.push_back(rec);
        x += w;
    }
}

void membar::drawSectionHeaders() {
    programHeadersRects.clear();
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(colors[drawTarget]);
    painter.setPen(QPen(borderColor, borderSize));
    if (borderSize <= 0)
        painter.setPen(Qt::NoPen);
    painter.setFont(QFont("Arial", fontSize, QFont::Bold));

    const double scale = width() / static_cast<double>(programHeaders[drawTarget].size);

    for (const auto &segment: sectionHeaders[drawTarget]) {
        painter.setPen(QPen(borderColor, borderSize));
        const uint64_t start = segment.addr - programHeaders[drawTarget].addr;
        auto rect = QRectF(static_cast<double>(start) * scale, 0, static_cast<double>(segment.size) * scale, height());
        painter.drawRect(rect);

        painter.setPen(Qt::white);
        if (QFontMetrics fm(QFont("Arial", fontSize, QFont::Bold)); rect.width() > fm.horizontalAdvance(segment.name)) {
            painter.setPen(fontColor);
            painter.drawText(rect, Qt::AlignCenter, segment.name);
        }

        programHeadersRects.push_back(rect);
    }
}
