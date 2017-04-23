#include "DigitalIO.h"


DigitalIO::DigitalIO (QWidget * p, const QString & name, const IO_Dir dir, const bool b)
  : QWidget (p), direction (dir), bistable(b) {
  on  = new QPixmap(QString::fromUtf8(":/red1"));
  off = new QPixmap(QString::fromUtf8(":/red0"));
  setObjectName (QString::fromUtf8("widget"));
  verticalLayout = new QVBoxLayout(this);
  verticalLayout->setObjectName      (QString::fromUtf8("verticalLayout"));
  verticalLayout->setContentsMargins (0, 0, 0, 0);
  label = new QLabel(this);
  label->setObjectName(QString::fromUtf8("label"));
  label->setPixmap(* off);
  label->setAlignment(Qt::AlignCenter);

  verticalLayout->addWidget(label);

  pushButton = new QPushButton(this);
  pushButton->setObjectName(QString::fromUtf8("pushButton"));
  QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  sizePolicy.setHorizontalStretch (0);
  sizePolicy.setVerticalStretch   (0);
  sizePolicy.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
  pushButton->setSizePolicy  (sizePolicy);
  pushButton->setMinimumSize (QSize(30,  30));
  pushButton->setMaximumSize (QSize(300, 30));
  pushButton->setText (name);
  if (direction == IO_Output) pushButton->setEnabled(false);

  verticalLayout->addWidget(pushButton);
  
  connect (pushButton, SIGNAL(pressed()),    this, SLOT(ButtPressed()));
  connect (pushButton, SIGNAL(released()),   this, SLOT(ButtReleased()));
  connect (this,       SIGNAL(IconChange()), this, SLOT(ChangeIcon()));

  state = false;
}
DigitalIO::~DigitalIO() {
  delete verticalLayout;
  delete label;
  delete pushButton;
  delete on;
  delete off;
}
void DigitalIO::ChangeIcon (void) {
  if (state) label->setPixmap (* on);
  else       label->setPixmap (* off);
}

void DigitalIO::SetState (bool s) {
  if (state == s) return;
  state = s;
  emit IconChange();
}
void DigitalIO::ButtPressed (void) {
  if (bistable) {
    SetState (true);
  } else {
    StateChange();
  }
}
void DigitalIO::ButtReleased (void) {
  if (bistable) {
    SetState(false);
  }
}

void DigitalIO::StateChange (void) {
  if (state) SetState (false);
  else       SetState (true);
}
void DigitalIO::Enable (bool b) {
  label->setEnabled (b);
}
