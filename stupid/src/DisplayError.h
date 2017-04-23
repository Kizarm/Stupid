#ifndef DISPLAYERROR_H
#define DISPLAYERROR_H

#include <QPlainTextEdit>

class DisplayError : public QPlainTextEdit {
    Q_OBJECT

  public:
    DisplayError(QWidget * parent);
    virtual ~DisplayError();
    void AddString (const QString & message);
    static void log (const char * message);
    static DisplayError * Instance;
  private slots:
    void highlightCurrentLine();
  signals:
    void lineError (int ln);
};

#endif // DISPLAYERROR_H
