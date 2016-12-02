#include <stdio.h>
#include "AnalogIO.h"
#include "iogbl.h"

AnalogINP::AnalogINP (QWidget * parent, const QString & iname) : QWidget(parent) {
  setObjectName(QString::fromUtf8 ("AnalogINP"));
  horizontalLayout = new QHBoxLayout(this);
  horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
  horizontalLayout->setContentsMargins(0, 0, 0, 0);
  name = new QLabel   (this);
  name->setObjectName (iname);
  QFont font;
  font.setBold(true);
  font.setWeight(75);
  name->setFont(font);

  horizontalLayout->addWidget(name);

  horizontalSlider = new QSlider(this);
  horizontalSlider->setObjectName(QString::fromUtf8("horizontalSlider"));
  horizontalSlider->setMaximum(255);
  horizontalSlider->setPageStep(16);
  horizontalSlider->setOrientation(Qt::Horizontal);

  horizontalLayout->addWidget(horizontalSlider);

  sval = new QLabel(this);
  sval->setObjectName(QString::fromUtf8("sval"));
  QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
  sizePolicy.setHorizontalStretch (0);
  sizePolicy.setVerticalStretch   (0);
  sizePolicy.setHeightForWidth(sval->sizePolicy().hasHeightForWidth());
  sval->setSizePolicy(sizePolicy);

  horizontalLayout->addWidget(sval);

  name->setText (iname);
  sval->setText ("000");
  value = 0;

  connect (horizontalSlider, SIGNAL(valueChanged(int)),    this, SLOT(SlideChange(int)));
  connect (this,             SIGNAL(ChangeValue(quint16)), this, SLOT(ValueChange(quint16)));
}

AnalogINP::~AnalogINP() {
  delete name;
  delete sval;
  delete horizontalLayout;
  delete horizontalSlider;

}

void AnalogINP::Enable (bool b) {
  setEnabled (b);
}
void AnalogINP::SetValue (uint16_t v) {
  fprintf (stderr, "[%d]: !!! Write to Analog Input\n", GIODescriptor.NetAddr);
  if (v == value) return;
  // return;
  emit ChangeValue (v);
}
void AnalogINP::SlideChange (int v) {
  QString s;
  s.sprintf ("%3d", v);
  sval->setText (s);
  value = v;
}
void AnalogINP::ValueChange (quint16 v) {
  SlideChange(v & 0xFF);
}

/* ******************************************************************************************/
AnalogOUT::AnalogOUT (QWidget * parent, const QString & iname) : QWidget (parent) {
  setObjectName(QString::fromUtf8("AnalogOUT"));
  horizontalLayout = new QHBoxLayout(this);
  horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
  horizontalLayout->setContentsMargins(0, 0, 0, 0);

  progressBar = new QProgressBar(this);
  progressBar->setObjectName(QString::fromUtf8("progressBar"));
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  sizePolicy.setHorizontalStretch (0);
  sizePolicy.setVerticalStretch   (0);
  sizePolicy.setHeightForWidth(progressBar->sizePolicy().hasHeightForWidth());
  progressBar->setSizePolicy (sizePolicy);
  progressBar->setMinimumSize(QSize(0, 20));
  progressBar->setMaximumSize(QSize(10000, 20));
  progressBar->setMaximum (255);
  progressBar->setValue   (0);
  progressBar->setFormat  ("%v");
  value = 0;

  horizontalLayout->addWidget(progressBar);

  name = new QLabel(this);
  name->setObjectName(QString::fromUtf8("name"));
  QFont font;
  font.setBold(true);
  font.setWeight(75);
  name->setFont(font);

  horizontalLayout->addWidget(name);
  
  name->setText (iname);

  connect (this, SIGNAL(ChangeValue(quint16)), this, SLOT(ValueChange(quint16)));
}
AnalogOUT::~AnalogOUT() {
  delete name;
  delete horizontalLayout;
  delete progressBar;
}
void AnalogOUT::Enable (bool b) {
  setEnabled (b);
}
void AnalogOUT::SetValue (quint16 v) {
  if (v == value) return;
  emit ChangeValue (v);
}
void AnalogOUT::ValueChange (quint16 v) {
  progressBar->setValue (v & 0xFF);
  value = v;
}

