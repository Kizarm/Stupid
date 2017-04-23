#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H
#include <QDialog>
#include <QWidget>
#include <QString>
#include <QCheckBox>

class Ui_InputDialog;
class Ui_Bit8Dialog;
class Ui_FindDialog;
class WatchElement;

class VirtualInput : public QDialog {
  Q_OBJECT
  public:
    VirtualInput  (QWidget * parent, WatchElement * e);
    virtual QString & NewValue (QString & dsc)=0;
    virtual ~VirtualInput () {};
  public:
    WatchElement   * elem;
    unsigned         base;
};

class InputDialog : public VirtualInput {
  Q_OBJECT
  public:
    InputDialog (QWidget * p, WatchElement * e);
    QString & NewValue  (QString & dsc);
    virtual ~InputDialog();
  protected:
    void SetTextValue (void);
  public slots:
    void accept (void);
    void BaseChanged (int);
    void TextChanged (const QString&);
  private:
    Ui_InputDialog * ui;
    unsigned         value;
//  unsigned         base;
    QString strval;
    QString strerr;
};

class Input8Bit: public VirtualInput {
  Q_OBJECT
  public:
    Input8Bit (QWidget * p, WatchElement * e, const char * dsc, int no, int count=8);
    QString & NewValue  (QString & dsc);
    virtual ~Input8Bit();
  protected:
    unsigned GetChecked (void);
    void SetChecked (unsigned);
  public slots:
    void accept  (void);
    void checked (bool);
  private:
    const unsigned   oct;
    QCheckBox      * bits;
    Ui_Bit8Dialog  * ui;
    unsigned         value;
    QString strval;
    QString strerr;
};

class FindDialog : public QDialog {
  Q_OBJECT
  public:
    FindDialog (QWidget * p);
    virtual ~FindDialog();
    QString & GetString (void);
  public slots:
    void accept  (void);
  private:
    Ui_FindDialog * ui;
    QString         context;
};
#endif // INPUTDIALOG_H
