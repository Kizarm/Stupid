#include "pesitem.h"
#include "watchelement.h"
#include "inputdialog.h"
//#include "mainwindow.h"
#include "viewwindow.h"
#include "DnlItem.h"

/** ***********************************************************************************/
XWord::XWord(bool fromFile) {
  word = new WatchXWord;
  item = new QTreeWidgetItem;
  word->isFromFile = fromFile;
}
void XWord::Set (QTreeWidgetItem * p, CPesNet * d, const char * dsc, unsigned int addr, int no) {
  word->setElement (d, addr);
  word->setInput   (new InputDialog (d->GetParent(), word));
  
  QString s (dsc);
  if (no >= 0) s.sprintf ("%s%d", dsc, no);
  item->setText    (0, s);
  item->setToolTip (0, s);
  item->setIcon (0, QIcon (":wrd"));
  QVariant v; v.setValue<WatchElement*>(word);
  item->setData (0, Qt::UserRole, v);
  p->addChild   (item);
}
XWord::~XWord() {
  delete item;
  delete word;
}
XByte::XByte() {
  byte = new WatchXByte;
  item = new QTreeWidgetItem;
}
void XByte::Set (QTreeWidgetItem * p, CPesNet * d, const char * dsc, unsigned int addr, int no) {
  byte->setElement (d, addr);
  byte->setInput   (new Input8Bit (d->GetParent(), byte, dsc, no));
  
  QString s (dsc);
  no *= 8;
  if (no >= 0) {
    s.sprintf ("%s%d-%d", dsc, no, no + 7);
    byte->SetAliases     (dsc, no, no + 7);
  }
  item->setToolTip (0, s);
  item->setText (0, s);
  item->setIcon (0, QIcon (":bit"));
  QVariant v; v.setValue<WatchElement*>(byte);
  item->setData (0, Qt::UserRole, v);
  p->addChild   (item);
}
XByte::~XByte() {
  delete item;
  delete byte;
}
SBit::SBit() {
  bit  = new WatchSTPBit;
  item = new QTreeWidgetItem;
}
void SBit::Set (QTreeWidgetItem * p, CPesNet * d, const char * dsc, unsigned int addr, int no) {
  bit->setElement (d, addr);
  bit->setInput   (new Input8Bit (d->GetParent(), bit, dsc, no, 1));
  
  QString s (dsc);
  if (no >= 0) s.sprintf ("%s%d", dsc, no);
  item->setText (0, s);
  item->setToolTip (0, s);
  item->setIcon (0, QIcon (":bit"));
  QVariant v; v.setValue<WatchElement*>(bit);
  item->setData (0, Qt::UserRole, v);
  p->addChild   (item);
}
SBit::~SBit() {
  delete item;
  delete bit;
}

/** ***********************************************************************************/

XWords::XWords (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc, const char * mark,
                unsigned int addr, unsigned int from, unsigned int count) {
  root = new QTreeWidgetItem (p);
  QString s (dsc);
  root->setText    (0, s);
  root->setToolTip (0, s);
  root->setIcon    (0, QIcon (ico));

  words = new XWord [count];
  unsigned base = addr + 2*from;
  for (unsigned i=0; i<count; i++) {
    words[i].Set (root, d, mark, base + 2*i, from + i);
  }
}
XWords::~XWords() {
//delete root;
  delete [] words;
}
XBytes::XBytes (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc, const char * mark,
                unsigned int addr, unsigned int from, unsigned int count) {
  root = new QTreeWidgetItem (p);
  QString s (dsc);
  root->setText    (0, s);
  root->setToolTip (0, s);
  root->setIcon    (0, QIcon (ico));

  bytes = new XByte [count];
  unsigned base = addr + from;
  for (unsigned i=0; i<count; i++) {
    bytes[i].Set (root, d, mark, base + i, from + i);
  }
}
XBytes::~XBytes() {
  delete [] bytes;
}
/** ***********************************************************************************/
System::System (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc) {
  root = new QTreeWidgetItem (p);
  QString s (dsc);
  root->setText    (0, s);
  root->setToolTip (0, s);
  root->setIcon    (0, QIcon (ico));

  reset = new SBit;  reset->Set (root, d, "Reset",        0x013E);
  turbo = new SBit;  turbo->Set (root, d, "Turbo",        0x013D);
  eesav = new SBit;  eesav->Set (root, d, "EESave",       0x00F6);
  eeloa = new SBit;  eeloa->Set (root, d, "EELoad",       0x00F7);
  eewrc = new XWord; eewrc->Set (root, d, "EEWriteCount", 0x014E);
  speed = new XWord; speed->Set (root, d, "Speed",        0x011E);
  myadr = new XWord; myadr->Set (root, d, "My Addr",      0x0164);
  slavm = new SBit;  slavm->Set (root, d, "Slave Mode",   0x00FB);
}
System::~System() {
  delete reset;
  delete turbo;
  delete eesav;
  delete eeloa;
  delete eewrc;
  delete speed;
  delete myadr;
  delete slavm;
}
Timer::Timer () {
  root = new QTreeWidgetItem;
}
void Timer::Set (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc, int no) {
  QString s;
  s.sprintf ("%s%d", dsc, no);
  root->setText    (0, s);
  root->setToolTip (0, s);
  root->setIcon    (0, QIcon (ico));
  p->addChild      (root);
  
  tim = new XWord; tim->Set (root, d, "T",   0x0100 + 2*no, no);
  ten = new SBit;  ten->Set (root, d, "TEN", 0x00C0 +   no, no);
  tdm = new SBit;  tdm->Set (root, d, "TDM", 0x00C8 +   no, no);
  tpa = new SBit;  tpa->Set (root, d, "TPA", 0x00D0 +   no, no);
  toe = new SBit;  toe->Set (root, d, "TOE", 0x00D8 +   no, no);
  tof = new SBit;  tof->Set (root, d, "TOF", 0x00E0 +   no, no);
  clk = new SBit;  clk->Set (root, d, "CLK", 0x00E8 +   no, no);
}

Timer::~Timer() {
  delete tim;
  delete ten;
  delete tdm;
  delete tpa;
  delete toe;
  delete tof;
  delete clk;
}
Timers::Timers (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc, int count) {
  root = new QTreeWidgetItem (p);
  QString s (dsc);
  root->setText    (0, s);
  root->setToolTip (0, s);
  root->setIcon    (0, QIcon (ico));
  
  tim = new Timer [count];
  for (int i=0; i<count; i++) {
    tim[i].Set (root, d, ":tim", "Timer", i);
  }
}
Timers::~Timers() {
  delete [] tim;
}
/** ***********************************************************************************/
XLocal::XLocal (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc) {
  root = new QTreeWidgetItem (p);
  QString s (dsc);
  root->setText    (0, s);
  root->setToolTip (0, s);
  root->setIcon    (0, QIcon (ico));

  regs = new XWords (root, d, ":reg", "Registers", "D", 0x0080, 0, 32);
  rels = new XBytes (root, d, ":rel", "Relays",    "M", 0x0208, 0, 8 );
}
XLocal::~XLocal() {
  delete regs;
  delete rels;
}
XPublic::XPublic (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc) {
  root = new QTreeWidgetItem (p);
  QString s (dsc);
  root->setText    (0, s);
  root->setToolTip (0, s);
  root->setIcon    (0, QIcon (ico));

  regs = new XWords (root, d, ":reg", "Registers", "D", 0x0080, 32, 32);
  rels = new XBytes (root, d, ":rel", "Relays",    "M", 0x0208, 8 , 8 );
}
XPublic::~XPublic() {
  delete regs;
  delete rels;
}
Absolute::Absolute (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc) {
  root = new QTreeWidgetItem (p);
  QString s (dsc);
  root->setText    (0, s);
  root->setToolTip (0, s);
  root->setIcon    (0, QIcon (ico));

  loc = new XLocal  (root, d, ":loc", "Local");
  pub = new XPublic (root, d, ":pub", "Public");
}
Absolute::~Absolute() {
  delete loc;
  delete pub;
}
/** ***********************************************************************************/
Settings::Settings (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc) {
  root = new QTreeWidgetItem (p);
  QString s (dsc);
  root->setText    (0, s);
  root->setToolTip (0, s);
  root->setIcon    (0, QIcon (ico));

  adcm = new XWord; adcm->Set (root, d, "AdcMode",   0x0134);
  cals = new SBit;  cals->Set (root, d, "CalibSave", 0x00FA);
  calb = new XWords (root, d, ":reg", "Calibrations", "Calib", 0x0124, 0, 8);
}
Settings::~Settings() {
  delete adcm;
  delete cals;
  delete calb;
}
Analog::Analog (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc) {
  root = new QTreeWidgetItem (p);
  QString s (dsc);
  root->setText    (0, s);
  root->setToolTip (0, s);
  root->setIcon    (0, QIcon (ico));

  inps = new XWords   (root, d, ":ani", "Inputs",  "I", 0x0000, 0, 8);
  outs = new XWords   (root, d, ":ano", "Outputs", "O", 0x0040, 0, 8);
  sets = new Settings (root, d, ":set", "Settings");
}
Analog::~Analog() {
  delete inps;
  delete outs;
  delete sets;
}
Digital::Digital (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc) {
  root = new QTreeWidgetItem (p);
  QString s (dsc);
  root->setText    (0, s);
  root->setToolTip (0, s);
  root->setIcon    (0, QIcon (ico));

  inps = new XBytes   (root, d, ":dic", "Inputs",  "X", 0x0200, 0, 4);
  outs = new XBytes   (root, d, ":doc", "Outputs", "Y", 0x0204, 0, 4);
}
Digital::~Digital() {
  delete inps;
  delete outs;
}
Peripheral::Peripheral (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc) {
  root = new QTreeWidgetItem (p);
  QString s (dsc);
  root->setText    (0, s);
  root->setToolTip (0, s);
  root->setIcon    (0, QIcon (ico));

  anl = new Analog  (root, d, ":anl", "Analog");
  dig = new Digital (root, d, ":dig", "Digital");
}
Peripheral::~Peripheral() {
  delete anl;
  delete dig;
}

/** ***********************************************************************************/
TDisplay::TDisplay (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc) {
  root = new QTreeWidgetItem (p);
  QString s (dsc);
  root->setText    (0, s);
  root->setToolTip (0, s);
  root->setIcon    (0, QIcon (ico));

  fmt = new XWord; fmt->Set(root, d, "Format",   0x0146);
  pos = new XWord; pos->Set(root, d, "Position", 0x0144);
}
TDisplay::~TDisplay() {
  delete fmt; delete pos;
}
Keyboard::Keyboard (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc) {
  root = new QTreeWidgetItem (p);
  QString s (dsc);
  root->setText    (0, s);
  root->setToolTip (0, s);
  root->setIcon    (0, QIcon (ico));

  kbcod = new XWord; kbcod->Set (root, d, "KbCode",     0x0148);
  kbdel = new XWord; kbdel->Set (root, d, "KbDelay",    0x014A);
  kbrep = new XWord; kbrep->Set (root, d, "KbRepeat",   0x014C);
  kbrpe = new SBit;  kbrpe->Set (root, d, "KbRepEn",    0x00F2);
  kbsnd = new SBit;  kbsnd->Set (root, d, "KbSound",    0x00F3);
}
Keyboard::~Keyboard() {
  delete kbcod; delete kbdel; delete kbrep;
  delete kbrpe; delete kbsnd;
}
Terminal::Terminal (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc) {
  root = new QTreeWidgetItem (p);
  QString s (dsc);
  root->setText    (0, s);
  root->setToolTip (0, s);
  root->setIcon    (0, QIcon (ico));

  dis = new TDisplay (root, d, ":dis", "Display" );
  key = new Keyboard (root, d, ":key", "Keyboard");
}
Terminal::~Terminal() {

}

/** ***********************************************************************************/
RealTime::RealTime (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc) {
  root = new QTreeWidgetItem (p);
  QString s (dsc);
  root->setText    (0, s);
  root->setToolTip (0, s);
  root->setIcon    (0, QIcon (ico));
  
  sec = new XWord; sec->Set  (root, d, "Seconds",   0x0110);
  min = new XWord; min->Set  (root, d, "Minute",    0x0112);
  hod = new XWord; hod->Set  (root, d, "Hour",      0x0114);
  day = new XWord; day->Set  (root, d, "Day",       0x0116);
  mon = new XWord; mon->Set  (root, d, "Month",     0x0118);
  rok = new XWord; rok->Set  (root, d, "Year",      0x011A);
  wek = new XWord; wek->Set  (root, d, "Week",      0x011C);
  hold = new SBit; hold->Set (root, d, "Hold",      0x00F0);
  clrs = new SBit; clrs->Set (root, d, "ClrSec",    0x00F1);
}
RealTime::~RealTime () {
  delete sec; delete min;  delete hod;
  delete day; delete mon;  delete rok;
  delete wek; delete hold; delete clrs;
}
NetVar::NetVar (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc) {
  root = new QTreeWidgetItem (p);
  QString s (dsc);
  root->setText    (0, s);
  root->setToolTip (0, s);
  root->setIcon    (0, QIcon (ico));

  scod = new XWord; scod->Set (root, d, "SendCode",   0x0154);
  sadr = new XWord; sadr->Set (root, d, "SendAddr",   0x0156);
  madr = new XWord; madr->Set (root, d, "MemAddr",    0x0158);
  badr = new XWord; badr->Set (root, d, "BitAddr",    0x015A);
  sdat = new XWord; sdat->Set (root, d, "SendData",   0x015C);
  sbit = new SBit;  sbit->Set (root, d, "SendBit",    0x00F9);
}
NetVar::~NetVar() {
  delete scod; delete sadr; delete madr;
  delete badr; delete sdat; delete sbit;
}

/** ***********************************************************************************/
Ally::Ally (QTreeWidget * p, CPesNet * d, int no) {
  root = new QTreeWidgetItem (p);
  SetStationNumber (no);
  root->setIcon    (0, QIcon(":ptr"));
  
  dnl     = new DnlItem    (root, d, ":per", "DnlFile");
  sys     = new System     (root, d, ":sys", "System");
  abs     = new Absolute   (root, d, ":abs", "Absolute");
  per     = new Peripheral (root, d, ":per", "Periferie");
  tim     = new Timers     (root, d, ":tim", "Timers", 8);
  rtc     = new RealTime   (root, d, ":rtc", "RTC");
  trm     = new Terminal   (root, d, ":trm", "Terminal");
  net     = new NetVar     (root, d, ":net", "NET");
  
}
Ally::~Ally() {
//delete root;
  delete dnl;
  delete sys;
  delete abs;
  delete per;
  delete tim;
  delete rtc;
  delete trm;
  delete net;
}
void Ally::SetStationNumber (int no) {
  QVariant qv   (no);
  root->setData (0, Qt::UserRole, qv);
  QString  s;
  s.sprintf ("Stanice %d", no);
  root->setText    (0, s);
  root->setToolTip (0, s);
}
