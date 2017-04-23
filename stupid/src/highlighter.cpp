#include <QtGui>

#include "highlighter.h"

Highlighter::Highlighter (QTextDocument * parent)
  : QSyntaxHighlighter (parent) {

  HighlightingRule rule;

  varFormat.setFontWeight (QFont::Bold);
  varFormat.setForeground (Qt::darkBlue);
  rule.pattern = QRegExp ("\\b[a-z][a-z0-9_]+\\b", Qt::CaseInsensitive);
  rule.format = varFormat;
  highlightingRules.append (rule);

  keywordFormat.setForeground (Qt::red);
  keywordFormat.setFontWeight (QFont::Bold);

  QStringList keywordPatterns;
  keywordPatterns << "\\bvar\\b" << "\\bconst\\b" << "\\bsymbol\\b"
                  << "\\bend\\b" << "\\barray\\b" << "\\bof\\b"
                  << "\\bif\\b" << "\\bthen\\b" << "\\belse\\b"
                  << "\\belseif\\b" << "\\bendif\\b" << "\\bcase\\b"
                  << "\\bendcase\\b" << "\\band\\b" << "\\bor\\b"
                  << "\\bxor\\b" << "\\bsubroutine\\b" << "\\bprocedure\\b"
                  << "\\bfunction\\b" << "\\breturn\\b" << "\\bbit\\b" << "\\bword\\b"
                  << "\\bint\\b" << "\\breal\\b" << "\\bin_bit\\b"
                  << "\\bin_word\\b" << "\\bdisplay\\b" << "\\$(lj|sj)";
  foreach (const QString &pattern, keywordPatterns) {
    rule.pattern = QRegExp (pattern, Qt::CaseInsensitive);
    rule.format = keywordFormat;
    highlightingRules.append (rule);
  }
  directiveFormat.setForeground (Qt::blue);
  directiveFormat.setFontWeight (QFont::Bold);
  QStringList directivePatterns;
  directivePatterns  << "#[ \t]*configuration" << "\\bnetaddr\\b" << "#[ \t]*include"
                     << "#[ \t]*(pragma)" << "#[ \t]*(codebegin)" << "#[ \t]*(databegin)";
  foreach (const QString &pattern, directivePatterns) {
    rule.pattern = QRegExp (pattern, Qt::CaseInsensitive);
    rule.format  = directiveFormat;
    highlightingRules.append (rule);
  }
  singleFormat.setFontWeight (QFont::Bold);
  singleFormat.setForeground (Qt::darkMagenta);
  rule.pattern = QRegExp ("\\b[diomxy][0-9]+\\b", Qt::CaseInsensitive);
  rule.format = singleFormat;
  highlightingRules.append (rule);

  doubleFormat.setFontWeight (QFont::Bold);
  doubleFormat.setForeground (Qt::darkMagenta);
  rule.pattern = QRegExp ("\\b[d][mxy][0-9]+\\b", Qt::CaseInsensitive);
  rule.format = doubleFormat;
  highlightingRules.append (rule);
  rule.pattern = QRegExp ("\\bt[0-7]{1}\\b", Qt::CaseInsensitive);
  highlightingRules.append (rule);
  rule.pattern = QRegExp ("\\b(clk|ten|tpa|tdm|toe|tof)[0-7]{1}\\b", Qt::CaseInsensitive);
  highlightingRules.append (rule);

  singleLineCommentFormat.setForeground (Qt::darkGray);
  rule.pattern = QRegExp ("//[^\n]*");
  rule.format = singleLineCommentFormat;
  highlightingRules.append (rule);
  multiLineCommentFormat.setForeground (Qt::darkGray);

  quotationFormat.setForeground (Qt::darkCyan);
  rule.pattern = QRegExp ("\"[^\"]*\"");
  rule.format = quotationFormat;
  highlightingRules.append (rule);
  rule.pattern = QRegExp ("\'[^\']{1}\'");
  highlightingRules.append (rule);

  //hexFormat.setFontItalic (true);
  //hexFormat.setBackground (Qt::yellow);
  hexFormat.setForeground (Qt::darkGreen);
  rule.pattern = QRegExp ("\\b0x[a-f0-9]+\\b", Qt::CaseInsensitive);
  rule.format = hexFormat;
  highlightingRules.append (rule);

  //decFormat.setFontItalic (true);
  decFormat.setForeground (Qt::darkGreen);
  rule.pattern = QRegExp ("\\b[0-9]+\\b");
  rule.format = decFormat;
  highlightingRules.append (rule);

  commentStartExpression = QRegExp ("\\{");
  commentEndExpression   = QRegExp ("\\}");

}

void Highlighter::highlightBlock (const QString & text) {
  foreach (const HighlightingRule &rule, highlightingRules) {
    QRegExp expression (rule.pattern);
    int index = expression.indexIn (text);
    while (index >= 0) {
      int length = expression.matchedLength();
      setFormat (index, length, rule.format);
      index = expression.indexIn (text, index + length);
    }
  }

  // neumi vnoreni
  setCurrentBlockState (0);
  int startIndex = 0;
  if (previousBlockState() != 1)
    startIndex = commentStartExpression.indexIn (text);
  while (startIndex >= 0) {
    int endIndex = commentEndExpression.indexIn (text, startIndex);
    int commentLength;
    if (endIndex == -1) {
      setCurrentBlockState (1);
      commentLength = text.length() - startIndex;
    } else {
      commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
    }
    //qDebug ("startIndex = %d, endIndex   = %d, length=%d", startIndex, endIndex, commentLength);
    setFormat (startIndex, commentLength, multiLineCommentFormat);
    startIndex = commentStartExpression.indexIn (text, startIndex + commentLength);
  }
}
// doplneno o export do html se zvyraznenim syntaxe - priklad z webu, funguje
void Highlighter::asHtml (QString & html) {
  // Create a new document from all the selected text document.
  QTextCursor cursor (document());
  cursor.select (QTextCursor::Document);
  QTextDocument * tempDocument (new QTextDocument);
  Q_ASSERT (tempDocument);
  QTextCursor tempCursor (tempDocument);

  tempCursor.insertFragment (cursor.selection());
  tempCursor.select (QTextCursor::Document);
  // Set the default foreground for the inserted characters.
  QTextCharFormat textfmt = tempCursor.charFormat();
  textfmt.setForeground (Qt::black);
  tempCursor.setCharFormat (textfmt);

  // Apply the additional formats set by the syntax highlighter
  QTextBlock start = document()->findBlock (cursor.selectionStart());
  QTextBlock end   = document()->findBlock (cursor.selectionEnd ());
  end = end.next();
  const int selectionStart = cursor.selectionStart();
  const int endOfDocument = tempDocument->characterCount() - 1;
  for (QTextBlock current = start; current.isValid() and current not_eq end; current = current.next()) {
    const QTextLayout * layout (current.layout());

    foreach (const QTextLayout::FormatRange &range, layout->additionalFormats()) {
      const int start = current.position() + range.start - selectionStart;
      const int end = start + range.length;
      if (end <= 0 or start >= endOfDocument) continue;
      tempCursor.setPosition (qMax (start, 0));
      tempCursor.setPosition (qMin (end, endOfDocument), QTextCursor::KeepAnchor);
      tempCursor.setCharFormat (range.format);
    }
  }

  // Reset the user states since they are not interesting
  for (QTextBlock block = tempDocument->begin(); block.isValid(); block = block.next())
    block.setUserState (-1);

  // Make sure the text appears pre-formatted, and set the background we want.
  tempCursor.select (QTextCursor::Document);
  QTextBlockFormat blockFormat = tempCursor.blockFormat();
  blockFormat.setNonBreakableLines (true);
  blockFormat.setBackground (Qt::white);
  tempCursor.setBlockFormat (blockFormat);

  // Finally retreive the syntax higlighted and formatted html.
  html = tempCursor.selection().toHtml();
  delete tempDocument;
} // asHtml
