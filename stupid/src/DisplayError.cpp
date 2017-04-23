#include <stdio.h>
#include <QTextBlock>
#include "DisplayError.h"

DisplayError * DisplayError::Instance = NULL;

DisplayError::DisplayError(QWidget * parent) : QPlainTextEdit (parent) {
  setReadOnly(true);
  
  QFont font;
  font.setFamily ("Courier");
  font.setBold   (true);
  font.setFixedPitch (true);
  font.setPointSize (12);
  font.setStretch   (QFont::Condensed);
  
  setFont (font);
  Instance = this;

  connect (this, SIGNAL (cursorPositionChanged()), this, SLOT (highlightCurrentLine()));
}

DisplayError::~DisplayError() {

}
void DisplayError::AddString (const QString & message) {
  insertPlainText (message);
}

void DisplayError::log (const char * message) {
  QString s = QString::fromUtf8 (message);
  s.remove (QChar('\r'));
  Instance->insertPlainText(s);
}
void DisplayError::highlightCurrentLine() {
  QList<QTextEdit::ExtraSelection> extraSelections;
  QColor lineColor = QColor (Qt::red).lighter (160);

  QTextCursor cursor = textCursor();
  QTextEdit::ExtraSelection selection;
  selection.format.setBackground (lineColor);
  selection.format.setProperty   (QTextFormat::FullWidthSelection, true);
  selection.cursor = cursor;
  selection.cursor.clearSelection();
  extraSelections.append (selection);
  setExtraSelections (extraSelections);

  int i = cursor.position();
  QTextBlock   b = document()->findBlock(i);
  if (!b.isValid()) return;

  const char * s = b.text().toLatin1().data();
//const char * s = b.text().toAscii().data();
  int k = 0, n = sscanf (s, "Chyba na radku %d", &k);
  if (n != 1) return;
  k -= 1;       // Qt ale ciluje od 0
  printf ("[%s] err at line = %d\n", s, k);
  emit lineError (k);
}
