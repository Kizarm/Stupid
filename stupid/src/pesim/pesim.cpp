#include <QSettings>
#include "pesim.h"
#include "ui_pesim.h"
#include "DigitalIO.h"
#include "AnalogIO.h"
#include "Loop.h"
#include "wrap.h"
#include "../../posix/simple/llvm/PesApi.h"

IODescriptors GIODescriptor;

SimDialog::SimDialog (QWidget * par, Qt::WindowFlags flags) 
  : QDialog (par, flags), ico(QPixmap(QString(":per"))) {
  ui = new Ui_SimDialog;
  ui->setupUi (this);
  
  QString s;
  GIODescriptor.NetAddr = GStationInfo.NetAddr;
  setWindowIcon  (ico);
  
  wrap = new QtWrap (this);
  GIODescriptor.wrap = wrap;
  
  bool en;
  
  for (unsigned i=0; i<MaxIO; i++) {
    s.sprintf ("X%d",i);
    DigitalIO * pi = new DigitalIO (this, s, IO_Input);
    en = GIODescriptor.inp[i].used;
    pi->Enable (en);
    ui->InpLayout->addWidget (pi);
    GIODescriptor.inp[i].io = pi;
    
    s.sprintf ("Y%d",i);
    DigitalIO * po = new DigitalIO (this, s, IO_Output);
    en = GIODescriptor.out[i].used;
    po->Enable (en);
    ui->OutLayout->addWidget (po);
    GIODescriptor.out[i].io = po;
  }
  
  for (unsigned i=0; i<MaxAD; i++) {
    s.sprintf ("I%d",i);
    AnalogINP * p = new AnalogINP (this, s);
    en = GIODescriptor.adc[i].used;
    p->Enable(en);
    ui->analogLayout->addWidget (p);
    GIODescriptor.adc[i].io = p;
  }
  
  line = new QFrame(this);
  line->setObjectName(QString::fromUtf8("line"));
  //line->setGeometry(QRect(190, 200, 118, 3));
  line->setFrameShape (QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  ui->analogLayout->addWidget(line);
  
  for (unsigned i=0; i<MaxAD; i++) {
    s.sprintf ("O%d",i);
    AnalogOUT * p = new AnalogOUT (this, s);
    en = GIODescriptor.dac[i].used;
    p->Enable(en);
    ui->analogLayout->addWidget (p);
    GIODescriptor.dac[i].io = p;
  }
  
  user = 0;
}
SimDialog::~SimDialog() {
  delete wrap;
  if (user) delete user;
  delete line;
  for (unsigned i=0; i<MaxIO; i++) {
    delete GIODescriptor.inp[i].io;
    delete GIODescriptor.out[i].io;
  }
  for (unsigned i=0; i<MaxAD; i++) {
    delete GIODescriptor.adc[i].io;
    delete GIODescriptor.dac[i].io;
  }
  delete ui;
}
void SimDialog::StartSimulator (void) {
  QString s;
  GIODescriptor.NetAddr = GStationInfo.NetAddr;
  s.sprintf("Automat # %d # DATA(%04X-%04X)",
            GIODescriptor.NetAddr, GStationInfo.DataBegin, GStationInfo.DataEnd);
  setWindowTitle (s);
  user = new Loop (this);
  show();
}

void SimDialog::closeEvent (QCloseEvent* event) {
  if (user) delete user;
  user = 0;
//qDebug("ExitLibPes");
  ExitLibPes();
//qDebug("Sim close()");
  QDialog::closeEvent (event);
}
