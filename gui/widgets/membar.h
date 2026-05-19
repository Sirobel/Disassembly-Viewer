//
// Created by finns on 10.03.26.
//

#ifndef DISASSEMBLY_VIEWER_MEMBAR_H
#define DISASSEMBLY_VIEWER_MEMBAR_H

#include <QSettings>
#include <QWidget>
#include <QTimer>


QT_BEGIN_NAMESPACE

namespace Ui {
    class membar;
}

QT_END_NAMESPACE

class membar : public QWidget {
    Q_OBJECT

public:
    explicit membar(QWidget *parent = nullptr);

    ~membar() override;

    struct Segment {
        QString name;
        uint64_t addr;
        uint64_t size;
    };

    void initializeBar(const std::vector<std::vector<Segment> > &secHeader, const std::vector<Segment> &progHeader);

    void refresh();

protected:
    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void leaveEvent(QEvent *event) override;

private:
    Ui::membar *ui;
    QSettings settings;

    std::vector<Segment> programHeaders;
    std::vector<std::vector<Segment> > sectionHeaders;
    QVector<QRectF> programHeadersRects;
    QVector<QPair<int,QRectF>> sectionHeadersRects;
    QVector<QColor> colors;
    int drawTarget;
    uint64_t maxSize;

    QTimer hoverTimer;
    int hoverIndex;
    QPoint mousePos;

    int fontSize = 0;
    QColor fontColor = Qt::white;
    int borderSize = 0;
    QColor borderColor = Qt::black;

    void drawAll();

    void drawSectionHeaders();
};


#endif //DISASSEMBLY_VIEWER_MEMBAR_H
