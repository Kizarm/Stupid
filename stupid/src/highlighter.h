#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class Highlighter : public QSyntaxHighlighter {
    Q_OBJECT

  public:
    Highlighter (QTextDocument * parent = 0);
    void asHtml (QString& html);

  protected:
    void highlightBlock (const QString & text);

  private:
    struct HighlightingRule {
      QRegExp pattern;
      QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;
 
    QTextCharFormat keywordFormat;
    QTextCharFormat directiveFormat;
    QTextCharFormat singleFormat;
    QTextCharFormat doubleFormat;
    QTextCharFormat varFormat;
    QTextCharFormat hexFormat;
    QTextCharFormat decFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
};

#endif
