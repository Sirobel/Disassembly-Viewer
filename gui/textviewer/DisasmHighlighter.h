//
// Created by finns on 21.04.26.
//

#ifndef DISASSEMBLY_VIEWER_DISASMHIGHLIGHTER_H
#define DISASSEMBLY_VIEWER_DISASMHIGHLIGHTER_H

#pragma once
#include <QSyntaxHighlighter>
#include <QRegularExpression>

class DisasmHighlighter : public QSyntaxHighlighter {
public:
    explicit DisasmHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct highlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<highlightingRule> rules;
};


#endif //DISASSEMBLY_VIEWER_DISASMHIGHLIGHTER_H
