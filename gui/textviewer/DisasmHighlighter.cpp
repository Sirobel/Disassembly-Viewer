//
// Created by finns on 21.04.26.
//

#include "DisasmHighlighter.h"

DisasmHighlighter::DisasmHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
    QTextCharFormat linkFormat;
    linkFormat.setForeground(QColor(0x4FC3F7));
    linkFormat.setFontUnderline(true);

    rules.append({QRegularExpression(R"(‌[^‌]+‌)"), linkFormat});
    rules.append({QRegularExpression(R"(#target\s+0x[0-9a-fA-F]+(?:\s+<.+>)?)"), linkFormat});
}

void DisasmHighlighter::highlightBlock(const QString &text) {
    for (const auto &[pattern, format]: rules) {
        QRegularExpressionMatchIterator it = pattern.globalMatch(text);
        while (it.hasNext()) {
            auto match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), format);
        }
    }
}
