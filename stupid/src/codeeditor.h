#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QObject>

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

class LineNumberArea;


class CodeEditor : public QPlainTextEdit {
    Q_OBJECT

  public:
    CodeEditor (QWidget * parent = 0);

    void lineNumberAreaPaintEvent (QPaintEvent * event);
    int lineNumberAreaWidth();
    void setUserTable (int size, bool change);

  protected:
    void resizeEvent   (QResizeEvent * event);
    void keyPressEvent (QKeyEvent    * event);

    void Indent    (void);
    bool TabChange (void);
  public slots:
    void setLinePos (int ln);
  private slots:
    void updateLineNumberAreaWidth (int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea (const QRect &, int);

  private:
    int       userTabSize;
    bool      userTabChange;
    QWidget * lineNumberArea;
};

class LineNumberArea : public QWidget {
  public:
    LineNumberArea (CodeEditor * editor) : QWidget (editor) {
      codeEditor = editor;
    }

    QSize sizeHint() const {
      return QSize (codeEditor->lineNumberAreaWidth(), 0);
    }

  protected:
    void paintEvent (QPaintEvent * event) {
      codeEditor->lineNumberAreaPaintEvent (event);
    }

  private:
    CodeEditor * codeEditor;
};

#endif
