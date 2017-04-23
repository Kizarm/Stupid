#include "inputdialog.h"
#include "ui_inputdialog.h"
#include "ui_bit8dialog.h"
#include "ui_finddialog.h"
#include "watchelement.h"
#include <QMutex>


VirtualInput::VirtualInput (QWidget * parent, WatchElement * e) : QDialog (parent) {
  elem = e;
  base = 16;
}
/** *****************************************************************************/

InputDialog::InputDialog (QWidget * p, WatchElement * e) : VirtualInput (p, e) {
  ui = new Ui_InputDialog;
  ui->setupUi (this);
  QVariant b2(2), b8(8), b10(10), b16(16);
  ui->comboBox->addItem (QString::fromUtf8("Dvojková" ),   b2);
  ui->comboBox->addItem (QString::fromUtf8("Osmičková"),   b8);
  ui->comboBox->addItem (QString::fromUtf8("Desítková"),   b10);
  ui->comboBox->addItem (QString::fromUtf8("Šestnácková"), b16);
  connect (ui->comboBox, SIGNAL(activated(int)),      this, SLOT(BaseChanged(int)));
  connect (ui->lineEdit, SIGNAL(textEdited(QString)), this, SLOT(TextChanged(QString)));
  
  ui->comboBox->setCurrentIndex (3);
  
  strerr = "Error";
  value  = 0;
}
InputDialog::~InputDialog() {
  delete ui;
}
void InputDialog::SetTextValue (void) {
  strval.setNum (value, base);
  ui->lineEdit->setText  (strval);
  ui->lineEdit->selectAll();
  ui->lineEdit->setFocus ();
}
void InputDialog::TextChanged (const QString & s) {
  //qDebug ("Text: %s", s.toAscii().data());
  bool result; unsigned tmp;
  tmp = s.toUInt (&result, base);
  if (result) {
    value = tmp;
  } else {
    if (value) {
      QString n;
      n.setNum (value, base);
      ui->lineEdit->setText(n);
    } else
      ui->lineEdit->clear();
  }
}
void InputDialog::BaseChanged (int n) {
  base = ui->comboBox->itemData(n).toUInt();
  // qDebug ("%d Selected : %d", n, base);
  SetTextValue();
}

QString & InputDialog::NewValue (QString & dsc) {
  QString s = dsc + " :";
  ui->label->setText (s);
  bool result = false;
  result = elem->getWatchValue (value);
  if (!result) return strerr;
  
  SetTextValue();
  
  exec ();
  
  elem->base = base;
  strval = elem->StrValue();
  return strval;
}
void InputDialog::accept (void) {
  QString s;
  bool result = false;
  s = ui->lineEdit->text();
  value = s.toUInt (&result, base);
  if (!result) return;
  result = elem->setWatchValue (value);
  if (!result) return;
  strval = elem->StrValue();
  qDebug ("accResult=%s", strval.toLatin1().data());
  
  close ();
}
/** *****************************************************************************/
Input8Bit::Input8Bit (QWidget * p, WatchElement * e, const char * dsc, int no, int count) 
  : VirtualInput (p, e), oct(count) {
  ui = new Ui_Bit8Dialog;
  ui->setupUi (this);
  strerr = "Error";
  QString s;
  bits = new QCheckBox [oct];
  QVBoxLayout * l = ui->verticalLayout;
  for (unsigned i = 0; i < oct; i++) {
    if (count > 1)
      s.sprintf ("%s%d", dsc, i + oct * no);
    else
      s.sprintf ("%s", dsc);
    bits[i].setText       (s);
    bits[i].setObjectName (s);
    bits[i].setParent     (this);
    l->addWidget (bits + i);
    connect (bits + i, SIGNAL(clicked(bool)), this, SLOT(checked(bool)));
  }
}
Input8Bit::~Input8Bit() {
  delete ui;
  delete [] bits;
}
QString & Input8Bit::NewValue (QString & dsc) {
  setWindowTitle (dsc);
  bool result = false;
  result = elem->getWatchValue (value);
  if (!result) return strerr;
  SetChecked (value);
  
  exec();
  strval = elem->StrValue();
  return strval;
}
void Input8Bit::accept (void) {
  close();
}
void Input8Bit::checked (bool) {
  value = GetChecked();
  bool result = elem->setWatchValue (value);
  if (!result) return;
  // qDebug ("Checked 0x%02X", value);
}
unsigned int Input8Bit::GetChecked (void) {
  unsigned result = 0;
  for (unsigned i=0; i<oct; i++) {
    if (bits[i].isChecked()) result |= (1U << i);
  }
  return result;
}
void Input8Bit::SetChecked (unsigned int n) {
  for (unsigned i=0; i<oct; i++) {
    if (n & (1U << i)) bits[i].setCheckState (Qt::Checked);
    else               bits[i].setCheckState (Qt::Unchecked);
  }

}
/** *****************************************************************************/
FindDialog::FindDialog (QWidget * p) : QDialog (p) {
  ui = new Ui_FindDialog;
  ui->setupUi (this);
  context.resize (0);
}
FindDialog::~FindDialog() {
  delete ui;
}
QString & FindDialog::GetString (void) {
  exec();
  qDebug ("Hledam: %s", context.toLatin1().data());
  return context;
}
void FindDialog::accept (void) {
  context = ui->lineEdit->text();
  close();
}

