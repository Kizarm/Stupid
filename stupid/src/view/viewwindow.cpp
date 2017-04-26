#include <QSettings>

#include "viewwindow.h"
#include "ui_viewwindow.h"
#include "pesitem.h"
#include "inputdialog.h"
#include "USBPes.h"
#include "watchelement.h"
#include "watchloop.h"
#include "mainwindow.h"
#include "DnlItem.h"
#include "cudppes.h"

ViewDialog::ViewDialog (MainWindow * par, Qt::WindowFlags flags) 
  : QDialog (par, flags), mainico (":per"), msgFont(), msgPalette(), list () {
  ui = new Ui_ViewDialog;
  ui->setupUi (this);
  parent = par;
  dnlvar = new DnlVariables;
  setWindowTitle (QString::fromUtf8("Qt4 Viewer"));
  setWindowIcon  (mainico);
  setting = parent->GetSettings();
  for (int n=0; n<30; n++) Station [n] = NULL;
  // nejde udelat v designeru
#ifndef QT_WIDGETS_LIB
  //ui->buttonFind->setShortcut(QApplication::translate("ViewDialog", "Alt+N", 0, QApplication::UnicodeUTF8));
  //ui->buttonStop->setShortcut(QApplication::translate("ViewDialog", "Alt+S", 0, QApplication::UnicodeUTF8));
#endif
  msgFont.setBold           (true);
  msgPalette.setColor       (QPalette::WindowText, Qt::red);
  ui->statusbar->setFont    (msgFont);
  ui->statusbar->setPalette (msgPalette);
  
  loop = new WatchLoop (this);
  // pes  = new CUSBPes   ();
  pes  = parent->GetNetDriver();
  pes->SetParent       (this);
  loop->setPes   (pes);
  loop->setList  (&list);
  
  ally = new Ally (ui->treeWidget, pes, 0);
  BuildTree ();
  BuildInfo ();
  
//connect (ui->treeWidget, SIGNAL(itemClicked       (QTreeWidgetItem * , int)), this, SLOT(TreeSingle(QTreeWidgetItem*,int)));
  connect (ui->treeWidget, SIGNAL(itemDoubleClicked (QTreeWidgetItem * , int)), this, SLOT(TreeDouble(QTreeWidgetItem*,int)));
  connect (ui->tableWidget,SIGNAL(itemDoubleClicked (QTableWidgetItem*)),       this, SLOT(TabSel    (QTableWidgetItem*)));
  connect (ui->buttonFind, SIGNAL(pressed()),                                   this, SLOT(FindVar()));
  connect (ui->buttonEnd,  SIGNAL(pressed()),                                   this, SLOT(close()));
  connect (ui->buttonStop, SIGNAL(pressed()),                                   this, SLOT(StartStop()));
  connect (ui->buttonRestart, SIGNAL(pressed()),                                this, SLOT(Restart()));
  connect (loop,           SIGNAL(VariableChanged(int)),                        this, SLOT(ChangeVal(int)));
  connect (loop,           SIGNAL(Terminated()),                                this, SLOT(Terminated()));
  
  QIcon netico (":/net");
  actionSelect_Station = new QAction (parent);
  actionSelect_Station->setIcon(netico);
  actionSelect_Station->setText(QString::fromUtf8("Výběr stanice"));
  parent->GetMenuNet()->addAction(actionSelect_Station);
  connect (actionSelect_Station, SIGNAL(triggered(bool)), this, SLOT(FindStation(bool)));
  
  ui->treeWidget->setEnabled (false);
  running = false;
  
  TestSettings();
}
ViewDialog::~ViewDialog() {
  delete dnlvar;
  delete actionSelect_Station;
  delete loop;
  delete ally;
  delete ui;
  if (!pes->isDisabled()) pes->COM_Close();
  for (int n=0; n<30; n++) {
    if (Station[n]) delete Station [n];
  }
}
void ViewDialog::StartWatching (void) {
  ComInit ();
  show();
}
void ViewDialog::TargetChanged (LLVMTypeMachine tgt) {
  if (!pes->isDisabled()) pes->COM_Close();
  delete pes;
  switch (tgt) {
    case MachineTypeLinux64:
      pes = new CUDPPes ();
      break;
    case MachineTypeCortexM0:
    case MachineTypeCortexM3:
    case MachineTypeCortexM4F:
    default:
      pes = new CUSBPes ();
      break;
  };
  parent->SetNetDriver (pes);
  pes->SetParent       (this);
  loop->setPes   (pes);
  qDebug("View: Target changed to %d", (int) tgt);
}

// posledni vybrana stanice se prida do Action.
bool ViewDialog::TestSettings (void) {
  bool result = false;
  setting->beginGroup("Station");
  int n = -1;
  if (setting->contains("Number")) {
    n = setting->value ("Number").toInt();
    // qDebug ("Settings - Station - Number = %d", n);
    pes->StationNum = 1;
    pes->StationList[0] = n;
    pes->SelectStation (n);
    result = AddActionNum (0, n);
  }
  setting->endGroup();
  return result;
}

void ViewDialog::FindStation (bool) {
  for (int n=0; n<30; n++) {
    if (!Station[n]) continue;
    //ui->menuNet->removeAction (Station [n]);
    delete Station [n]; Station [n] = NULL;
  }
  QString s;
  // qDebug ("Find Station");
  pes->NetScan();
  int n = pes->StationNum;
  s.sprintf ("Nalezeno %d stanic", n);
  if (!n) ui->statusbar->setText (QString::fromUtf8("Nenalezena žádná aktivní stanice"));
  else    ui->statusbar->setText (s);
  for (int i=0; i<n; i++) {
    int k = pes->StationList[i];
    AddActionNum (i, k);
  }
}
bool ViewDialog::AddActionNum (int ord, int addr) {
  QString s;
  qDebug ("Nalezena stanice %d", addr);
  s.sprintf ("Stanice %d", addr);
  Station[ord] = new QAction (this);
  Station[ord]->setCheckable (true);
  Station[ord]->setText (s);
  Station[ord]->setIcon  (mainico);
  parent->GetMenuNet()->addAction (Station[ord]);
  connect (Station[ord], SIGNAL(triggered(bool)), this, SLOT(SetStation(bool)));
  return true;
}

void ViewDialog::SetStation (bool) {
  for (int i=0; i<30; i++) {
    if (Station[i]) {
      QAction * a = Station[i];
      if (a->isChecked()) {
        uint8_t addr = pes->StationList[i];
        ally->SetStationNumber (addr);
        pes ->SelectStation    (addr);
        if (!GetStationInfo    (addr)) {
          ui->statusbar->setText(QString::fromUtf8(
            "Vybraná stanice neodpovídá, zkuste znovu menu Net -> Select Station"));
          return;
        }
        setting->beginGroup("Station");
        setting->setValue  ("Number", addr);
        setting->endGroup();
        ui->treeWidget->setEnabled (true);
        GetRunState ();
        loop->Start (running);
      }
    }
  }
}
void ViewDialog::Progress (int n) {
  ui->progressBar->setValue(n);
  return;
  /*
  QString s;
  s.sprintf ("Prohledavam %d", n);
  ui->statusbar->setText(s);
  */
}

bool ViewDialog::ComInit (void) {
  pes->COM_Init (456786);
  if (pes->isDisabled()) {
    actionSelect_Station->setEnabled(false);
    ui->buttonFind->setEnabled      (false);
    ui->statusbar->setText (QString::fromUtf8 ("USB kabel nelze inicializovat - zkontroluj připojení"));
    return false;
  }
  int no = pes->AddrTarget;
  pes->SelectStation (no);
  //pes->NetBreak();               // chyba - poruseni pipe
  //pes->USBReset();               // chyba - poruseni pipe
  //pes->SetBaudRate (57600);      // chyba - poruseni pipe
  //uint16_t br = pes->BaudScan(); // chyba - poruseni pipe
  return true;
}

void ViewDialog::BuildTree (void) {
  ui->tableWidget->setColumnCount (2);
  ui->tableWidget->setColumnWidth (0, 100);
  ui->tableWidget->setColumnWidth (1, 96 );
  QStringList l;
  l << QString::fromUtf8("Proměnná") << QString::fromUtf8("Hodnota");
  ui->tableWidget->setHorizontalHeaderLabels (l);
  ui->tableWidget->setRowCount    (0);
}
void ViewDialog::BuildInfo (void) {
  ui->tableInfo->setColumnCount (2);
  ui->tableInfo->setRowCount    (12);
  ui->tableInfo->setColumnWidth (0,140);
  ui->tableInfo->setColumnWidth (1,110);
  QTableWidgetItem * run = new QTableWidgetItem (QString::fromUtf8(" "));
  run->setFont       (msgFont);
  QBrush b           (Qt::blue);
  run->setForeground (b);
  
  ui->tableInfo->setItem (0, 0, new QTableWidgetItem (QString::fromUtf8("Stav")));
  ui->tableInfo->setItem (0, 1, run);
  
  ui->tableInfo->setItem (1, 0, new QTableWidgetItem (QString::fromUtf8("Typ kódu")));
  ui->tableInfo->setItem (1, 1, new QTableWidgetItem (QString::fromUtf8(" ")));
  
  ui->tableInfo->setItem (2, 0, new QTableWidgetItem (QString::fromUtf8("Kontrolní součet")));
  ui->tableInfo->setItem (2, 1, new QTableWidgetItem (QString::fromUtf8(" ")));
  
  ui->tableInfo->setItem (3, 0, new QTableWidgetItem (QString::fromUtf8("Typ Automatu")));
  ui->tableInfo->setItem (3, 1, new QTableWidgetItem (QString::fromUtf8(" ")));
  
  ui->tableInfo->setItem (4, 0, new QTableWidgetItem (QString::fromUtf8("Sériové číslo")));
  ui->tableInfo->setItem (4, 1, new QTableWidgetItem (QString::fromUtf8(" ")));
  
  ui->tableInfo->setItem (5, 0, new QTableWidgetItem (QString::fromUtf8("Verze BIOSu")));
  ui->tableInfo->setItem (5, 1, new QTableWidgetItem (QString::fromUtf8(" ")));
  
  ui->tableInfo->setItem (6, 0, new QTableWidgetItem (QString::fromUtf8("Adresa Stanice")));
  ui->tableInfo->setItem (6, 1, new QTableWidgetItem (QString::fromUtf8(" ")));
  
  ui->tableInfo->setItem (7, 0, new QTableWidgetItem (QString::fromUtf8("Kód")));
  ui->tableInfo->setItem (7, 1, new QTableWidgetItem (QString::fromUtf8(" ")));
  
  ui->tableInfo->setItem (8, 0, new QTableWidgetItem (QString::fromUtf8("Data")));
  ui->tableInfo->setItem (8, 1, new QTableWidgetItem (QString::fromUtf8(" ")));
  
  ui->tableInfo->setItem (9, 0, new QTableWidgetItem (QString::fromUtf8("Počet Downloadů")));
  ui->tableInfo->setItem (9, 1, new QTableWidgetItem (QString::fromUtf8(" ")));
  
  ui->tableInfo->setItem (10, 0, new QTableWidgetItem (QString::fromUtf8("Poslední Download")));
  ui->tableInfo->setItem (10, 1, new QTableWidgetItem (QString::fromUtf8(" ")));
  
  ui->tableInfo->setItem (11, 0, new QTableWidgetItem (QString::fromUtf8("Programová paměť")));
  ui->tableInfo->setItem (11, 1, new QTableWidgetItem (QString::fromUtf8(" ")));
  
  QBrush brush (QColor(255,196,255));
  for (int i=0; i<12; i++) {
    ui->tableInfo->setRowHeight (i, 20);
    QTableWidgetItem * it = ui->tableInfo->item (i, 0);
    it->setFlags (Qt::ItemIsEnabled);
    it->setBackground (brush);
  }
  brush.setColor (QColor(255,255,196));
  for (int i=0; i<12; i++) {
    QTableWidgetItem * it = ui->tableInfo->item (i, 1);
    it->setFlags (Qt::ItemIsEnabled);
    it->setBackground (brush);
  }
}
/// Unused
void ViewDialog::TreeSingle (QTreeWidgetItem * i, int) {
  QVariant qv = i->data (0, Qt::UserRole);
  if (qv.isNull())  return;
  bool result;
  unsigned address = qv.toUInt (&result);
  if (!result)      return;
  GetStationInfo (address);
}

void ViewDialog::TreeDouble (QTreeWidgetItem * i, int n) {
  if (i->child (n)) return;
  QVariant qv = i->data (0, Qt::UserRole);
  if (qv.isNull())  return;
  WatchElement * we = qv.value<WatchElement*>();
  if (!we)          return;
  
  bool Succsess   = false;
  unsigned int pn = 0; int k = 0;
  // zamykat operace nad USB a manipulaci s listem
  loop->lock (true);
  do {
    Succsess = we->getWatchValue (pn);
    if (!Succsess) break;
    Succsess = ! list.contains   (we);
    if (!Succsess) break;
    list.append          (we);
    k = list.lastIndexOf (we);
  } while (false);
  loop->lock (false);
  
  if (!Succsess) return;
  if (k < 0)     return;
  QString s = i->text(n);
  qDebug ("Selected: %s, %d - k=%d", s.toLatin1().data(), n, k);
  ui->tableWidget->insertRow (k);
  QTableWidgetItem * name  = new QTableWidgetItem (s);
  name->setFlags (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  ui->tableWidget->setItem   (k, 0, name);
  //s.sprintf ("0x%04X", pn);
  QTableWidgetItem * value = new QTableWidgetItem (we->StrValue());
  value->setFlags (Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  ui->tableWidget->setItem   (k, 1, value);
  
}
void ViewDialog::TabSel (QTableWidgetItem * i) {
  int c = i->column ();
  int r = i->row    ();
  if (!c) {             // tohle bude mazat radek list a table jsou synchronni, stejne
    ui->tableWidget->removeRow (r);
    loop->lock (true);
    list.removeAt              (r);
    loop->lock (false);
    return;
  }
  QTableWidgetItem * i0 = ui->tableWidget->item (r, 0);
  QString s  = i ->text();
  QString s0 = i0->text();
  qDebug ("%d:%d -> %s:%s", r, c,  s0.toLatin1().data(), s.toLatin1().data());
  loop->lock (true);
  do {
    WatchElement * we = list.at(r); if (!we) break;
    VirtualInput * vi = we->input;  if (!vi) break;
    s = vi->NewValue (s0);
    i->setText (s);
  } while (false);
  loop->lock (false);
}
//  #include <stdio.h>
#include "PesDefines.h"
bool ViewDialog::GetStationInfo (unsigned int station) {
  qDebug ("Info: %d", station);
  QString s;
  char    buf [256];
  QTableWidget * tw = ui->tableInfo;
  QTableWidgetItem * twi;
  
  loop->lock (true);
  twi = tw->item (6,1);
  s.sprintf ("%d", station);
  twi->setText   (s);
  pes->SelectStation (station);
  bool result = pes->ReliableSend (Cm_GetACK);
  if (!result) {
    loop->lock (false);
    return false;
  }
  result = pes->GetStpVersion (buf);
  if (result) {
    s = buf; twi = tw->item (1,1); twi->setText (s);
  }
  result = pes->GetNetVersion (buf);
  if (result) {
    s = buf; twi = tw->item (5,1); twi->setText (s);
  }
  uint8_t ex;
  result = pes->ReadEEByte (EE_CodeMemCfg, ex);
  if (result) {
    s = pes->GetProgMemType (ex, buf); twi = tw->item (7, 1); twi->setText (s);
  }
  result = pes->ReadEEByte (EE_DataMemCfg, ex);
  if (result) {
    s = pes->GetDataMemType (ex, buf); twi = tw->item (8, 1); twi->setText (s);
  }
  result = pes->GetPioName  (buf);
  if (result) {
    s = buf; twi = tw->item (3, 1); twi->setText (s);
  }
  uint16_t dct;
  result = pes->GetDownCount (dct);
  if (result) {
    s.sprintf ("%d", dct); twi = tw->item (9,1); twi->setText (s);
  }
  result = pes->GetLastDown  (buf, 64);
  if (result) {
    s = buf; twi = tw->item (10, 1); twi->setText (s); twi->setToolTip (s);
  }
  result = pes->GetLockState (buf);
  if (result) {
    s = buf; twi = tw->item (11, 1); twi->setText (s);
  }
  /* TODO : rozseparovat informace z EEPROM : Serial Number
  */
  unsigned bd = pes->BaudScan();   // vrati BaudRate z EEPROM stanice
  if (pes->isSuccsess()) {
    QString s;
    s.sprintf ("BaudRate = %d Bd", bd);
    ui->lineBaud->setText (s);
  }
  
  char StpName [13]; // Jmeno programu
  if (!pes->ReadEEBlock ((unsigned char *) StpName, 30, 8)) {
    qDebug ("Error ReadEEBlock");
  } else {
    StpName[8] = '\0';  // pro jistotu
    strcat (StpName, ".dnl");
    ui->lineFile->setText (StpName);
  }
  
  loop->lock (false);
  return true;
}
void ViewDialog::ChangeVal (int i) {
  // qDebug ("Changed : 0x%04X, %d", val, i);
  QTableWidgetItem * twi = ui->tableWidget->item (i, 1);
  WatchElement     * we  = list.at (i);
  QString s = we->StrValue();
  twi->setText (s);
}
void ViewDialog::Restart (void) {
  ui->statusbar->setText (" ");
  if ( pes->isDisabled()) return;
  if (!pes->StationNum)   return;
  if (!pes->SetStupidState (StupidStop)) {
    ui->statusbar->setText ("Nelze zastavit Simple");
    return;
  }
  loop->Delay (200);
  if (!pes->SetStupidState (StupidReset)) {
    ui->statusbar->setText ("Nelze zresetovat Simple");
    return;
  }
  loop->ForceStop ();
  pes ->COM_Close ();
  if (!ComInit()) return;
  loop->Delay (200);
  GetRunState ();
  loop->Start (running);
  qDebug ("Restart OK.");
}
void ViewDialog::GetRunState (void) {
  ui->statusbar->setText (" ");
  uint8_t state = 0;
  if (pes->isDisabled()) return;
  if (!pes->StationNum)  return;
  loop->lock (true);
  if (!pes->GetStatus (state)) {
    ui->statusbar->setText ("Nelze zjistit stav automatu");
    loop->lock (false);  return;
  }
  char const * text = "Stop";
  switch (state & 0xC0) {
    case 0x40: text = "Run";   break;
    case 0xC0: text = "Pause"; break;
    default : break;
    //case 0x80: text = "Stop";  break;
    //case 0x00: text = "Stop";  break;
  }
  if (state & 0x80) running = false;
  else              running = true;
  loop->lock  (false);
  qDebug ("GetRunState = %d (State 0x%02X)", running, state);
  QTableWidget     * tw  = ui->tableInfo;
  QTableWidgetItem * twi = tw->item (0, 1);
  twi->setText (text);
  if (running) {
    ui->buttonStop->setText ("&Stop");
  } else {
    ui->buttonStop->setText ("&Start");
  }
  twi = tw->item (2, 1);
  if (state & 0x100) {
    twi->setText ("ERROR");
  } else {
    twi->setText ("O.K.");
  }
}

void ViewDialog::StartStop (void) {
  ui->statusbar->setText (" ");
  GetRunState();        // podivej se zda bezi
  loop->lock (true);
  if (running) {        // pokud ano - zastav
    // hlasky zatim nebrat vazne - neni dodelan konecny status prikazu
    if (!pes->SetStupidState (StupidStop)) ui->statusbar->setText ("Stupid not halted !");
  } else {              // pokud ne, spust
    if (!pes->SetStupidState (StupidRun))  ui->statusbar->setText ("Stupid not running !");
  }
  loop->lock  (false);
  loop->Start (!running);       // spust smycku (negace), ale nemen running, stav se zjisti priste
  loop->Delay (200);
  GetRunState ();
}
void ViewDialog::Terminated (void) {
  qDebug ("Unexpected WatchLoop Terminated = %d", running);
  ui->statusbar->setText (QString::fromUtf8("Neočekávané ukončení sledování"));
  running = false;
  ui->buttonStop->setText ("&Start");
}
void ViewDialog::FindVar (void) {
  FindDialog dlg (this);
  QString s = dlg.GetString();
  if (s.isEmpty()) return;
  QTreeWidgetItem * ee = ally->getRoot();
  QTreeWidgetItem * rs = SearchVar (ee, s);
  if (!rs) {
    s.prepend (QString::fromUtf8("Nelze najít "));
    ui->statusbar->setText(s);
    return;
  }
  rs->setSelected (true);
  Expand (rs);
}
bool ViewDialog::ItemContains (const QTreeWidgetItem * item, const QString & find) {
  QString ms = item->text (0);
  if (! find.compare (ms, Qt::CaseInsensitive)) return true;
  QVariant qv = item->data (0, Qt::UserRole);
  if (qv.isNull())     return false;
  WatchElement * we = qv.value<WatchElement*>();
  if (!we)             return false;
  if (!we->HasAlias()) return false;
  return we->AliasContains (find);
}

// rekurze
QTreeWidgetItem * ViewDialog::SearchVar (QTreeWidgetItem * e, const QString & fs) {
  int nc = e->childCount();
  // qDebug ("%s Has %d child", e->text(0).toLatin1().data(), nc);
  if (ItemContains (e, fs)) {
    qDebug ("Nalezeno: %s v %s", fs.toLatin1().data(), e->text(0).toLatin1().data());
    return e;
  }
  if (!nc)  return NULL;
  QTreeWidgetItem * ch, * rs;
  for (int i=0; i<nc; i++) {
    ch = e->child  (i);
    rs = SearchVar (ch, fs);
    if (rs) return rs;
  }
  return NULL;
}
// rekurze
void ViewDialog::Expand (QTreeWidgetItem * e) {
  QTreeWidgetItem * p = e->parent();
  if (p) {
    p->setExpanded(true);
    Expand (p);
  }
}
bool ViewDialog::WatchStart (bool on) {
  if (on) {
    // restore state dodelat ???
    SetStation(true);
    return true;
  } else {
    loop->ForceStop();
    return true;
  }
}
void ViewDialog::RemoveFileWatchs (void) {
  if (list.isEmpty()) return;
  int i=0, n;
  for (;;) {
    n = list.size()-1;
    if (i > n) break;
    WatchElement * e = list.at(i);
    if (e->isFromFile) {
      //-printf ("strict remove watch at %d\n", i);
      ui->tableWidget->removeRow (i);
      loop->lock (true);
      list.removeAt              (i);
      loop->lock (false);
    } else i++;
  }
}
void ViewDialog::AddFileWatchs (const QString & filename) {
  dnlvar->readFile (filename);
  RemoveFileWatchs();
  dnlvar->addTree ();

}
void ViewDialog::SetDnlItemPointer (DnlItem * ptr) {
  if (!dnlvar) {
    return;
  }
  dnlvar->SetDnlItem (ptr);
}

