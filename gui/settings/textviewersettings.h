//
// Created by finns on 16.03.26.
//

#ifndef DISASSEMBLY_VIEWER_TEXTVIEWERSETTINGS_H
#define DISASSEMBLY_VIEWER_TEXTVIEWERSETTINGS_H

#include <QWidget>


QT_BEGIN_NAMESPACE

namespace Ui {
    class textviewersettings;
}

QT_END_NAMESPACE

class textviewersettings : public QWidget {
    Q_OBJECT

public:
    explicit textviewersettings(QWidget *parent = nullptr);

    ~textviewersettings() override;

private:
    Ui::textviewersettings *ui;
};


#endif //DISASSEMBLY_VIEWER_TEXTVIEWERSETTINGS_H