#include <QtCore>
#include <QtGui>
/* QT5 support */
#ifdef QT_WIDGETS_LIB
#include <QDialog>
#include <QFileDialog>
#include <QIcon>
#include <QComboBox>
#endif // DQT_WIDGETS_LIB
#ifdef QT_PRINTSUPPORT_LIB
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#endif // QT_PRINTSUPPORT_LIB
#include <QMessageBox>

#include "mainwindow.h"
#include "viewwindow.h"
#include "Loader.h"
#include "USBPes.h"
#include "platform.h"
#include "../posix/simple/llvm/PesApi.h"

static const char * fileEncoding = "UTF-8";
//! [0]
MainWindow::MainWindow (QWidget * parent)
  : QMainWindow (parent), msgPalette(), errPalette() {
  ui = new Ui_MainWindow;
  ui->setupUi (this);
  msgPalette.setColor (QPalette::WindowText, Qt::blue);
  errPalette.setColor (QPalette::WindowText, Qt::red);
  
  currentFileName.clear();
  currdnlFileName.clear();
  setupFileMenu();
  setupEditor();

  setCentralWidget (editor);
  setWindowTitle (tr ("Stupid Editor"));
  setWindowIcon  (QIcon(":per"));

  setupCompiler();
  // Propojení zobrazení chyby překladače a nastavením kurzoru editoru.
  connect (err, SIGNAL (lineError (int)), editor, SLOT (setLinePos (int)));
  
  pes      = new CUSBPes ();
  menuNet  = new QMenu (tr ("Net"), this);
  menuBar()->addMenu  (menuNet);
  setting  = new QSettings ("KizarmSoft", "Stupid");
  viewer   = new ViewDialog (this);
  // pes      = viewer->getPes ();
  simulace = new SimDialog  (this);
  acWiew   = ui->toolBar->addAction (QIcon(":diag"), tr ("Viewer"), viewer, SLOT (StartWatching()));
  acWiew->setShortcut (QKeySequence (Qt::Key_F9));
  TestSettings();
  loader   = new CLoaderDlg (pes, this);
  acLoad   = ui->toolBar->addAction (QIcon(":load"), tr ("Loader"), this,   SLOT (LoadDnl()));
  acLoad->setShortcut (QKeySequence (Qt::Key_F10));
  connect (loader, SIGNAL(SendMessage(QString)), this, SLOT (rcvMsg(QString)));
  connect (loader, SIGNAL(finished()),           this, SLOT (DownloadEnd(  )));
  setupHelpMenu();
  setupTarget  ();
}
//! [0]
MainWindow::~MainWindow () {
  delete loader;
  delete simulace;
  delete viewer;
  delete setting;
  delete menuNet;
  delete pes;
  delete err;
  delete ui;
  delete psel;
}
void MainWindow::Visibility (void) {
  acLoad->setVisible(true);
  acWiew->setVisible(true);
  switch (cf.F.TGT) {
    case MachineTypeCortexM0:
    case MachineTypeCortexM3:
    case MachineTypeCortexM4F:
      acLoad->setVisible(false);
    case MachineTypeLinux64:
      //acWiew->setVisible(false);
      break;
    default:
      break;
  }
}

void MainWindow::setupTarget() {
  psel = new QComboBox (this);
  psel->addItem(QString::fromUtf8("Nativní kód (*.dll)"));
  psel->addItem(QString::fromUtf8("ARM Cortex M0"));
  psel->addItem(QString::fromUtf8("ARM Cortex M3"));
  psel->addItem(QString::fromUtf8("ARM Cortex M4F"));
  psel->addItem(QString::fromUtf8("experimental WASM"));
  psel->addItem(QString::fromUtf8("DEFAULT x52"));
  psel->setToolTip(QString::fromUtf8("Výběr cílové architektury"));
  connect (psel, SIGNAL(activated(int)), this, SLOT(changeTarget(int)));
  int n = setting->value("Target").toInt();
  psel->setCurrentIndex(n);
  cf.F.TGT = (LLVMTypeMachine) n;
  Visibility();
  viewer->TargetChanged (cf.F.TGT);
  
  ui->toolBar->addSeparator();
  ui->toolBar->addWidget(psel);
}
void MainWindow::changeTarget (int n) {
  qDebug("Target changed to %d", n);
  setting->setValue("Target",  n);
  cf.F.TGT = (LLVMTypeMachine) n;
  Visibility();
  viewer->TargetChanged (cf.F.TGT);
}

void MainWindow::TestSettings (void) {
  setting->beginGroup("File");
  QString name = setting->value("Name").toString();
  setting->endGroup();
  if (name.isEmpty()) return;
  qDebug ("saved filename = %s\n", name.toLatin1().data());
  QFile file (name);
  if (!file.exists()) return;
  if (!file.open (QFile::ReadOnly | QFile::Text)) return;
  QTextStream in(&file);
  in.setCodec(fileEncoding);
  editor->setPlainText (in.readAll());
  err->log (name.toLatin1().constData());
  currentFileName = name;
}
void MainWindow::closeEvent (QCloseEvent * event) {
  if (currentFileName.isEmpty()) {
    event->accept();
    return;
  }
  //-qDebug("Close Event");
  if (assertFileChange(QString::fromUtf8("Uložit ?\n"))) {
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::about() {
  QMessageBox::about (this, tr ("About Stupid Editor"),
                      tr ("<p>Tento <b>Stupid Editor</b> je pokus o POSIX prekladac a " \
                          "editor jazyka SIMPLE s primitivnim zvyraznenim syntaxe.</p>"));
}
static const char * codeSnippet = "// Tato direktiva určuje konfiguraci automatu, pro kterou je program určen\n"
                                  "//(na základě této direktivy dojde k optimálnímu nastavení adresy programu a dat)\n"
                                  "#Configuration \"KIZARM\"\nNetAddr (1)\n\nend\n";
void MainWindow::newFile() {
  currentFileName.clear();
  editor->clear();
  editor->setPlainText(QString::fromUtf8(codeSnippet));
  err->clear();
}
static const char * filetypes = "STP Files (*.stp)";

void MainWindow::openFile (const QString & path) {
  QString fileName = path;

  if (fileName.isEmpty() || fileName.isNull()) {
    fileName = QFileDialog::getOpenFileName (this,
               tr ("Open File"), "", filetypes);
  }
  if (!fileName.isEmpty()) {
    currentFileName = fileName;
    QFile file (fileName);
    if (file.open (QFile::ReadOnly | QFile::Text)) {
      QTextStream in(&file);
      in.setCodec(fileEncoding);
      editor ->setPlainText (in.readAll());
      setting->beginGroup("File");
      setting->setValue("Name", fileName);
      setting->endGroup();
    }
  }
}
void MainWindow::saveFile (const QString & path) {
  QString fileName = path;

  if (fileName.isEmpty() || fileName.isNull())
    fileName = QFileDialog::getSaveFileName (this,
               tr ("Save File"), "", filetypes);

  if (!fileName.isEmpty()) {
    currentFileName = fileName;
    QFile file (fileName);
    if (file.open (QFile::WriteOnly | QFile::Truncate)) {
      QTextStream out(&file);
      out.setCodec(fileEncoding);
      out << editor->toPlainText();
      out.flush();
      file.close();
      dispMsg (fileName + " saved");
      setting->beginGroup("File");
      setting->setValue("Name", fileName);
      setting->endGroup();
    }
  }
}
void MainWindow::saveCurr (void) {
  if (editor->document()->isModified()) {
    saveFile (currentFileName);
    editor->document()->setModified(false);
  }
}

void MainWindow::exptHtml (const QString & path) {
  QString fileName = path;

  if (fileName.isNull())
    fileName = QFileDialog::getSaveFileName (this,
               tr ("Export File as HTML"), "", "HTML Files (*.html)");

  if (!fileName.isEmpty()) {
    QFile file (fileName);
    if (file.open (QFile::ReadWrite | QFile::Text | QFile::Truncate)) {
      QString s;
      s = editor->toPlainText();
      highlighter->asHtml (s);
//    file.write (s.toAscii());
      file.write (s.toLatin1());
      dispMsg (fileName + " exported");
    }
  }
}
void MainWindow::printDoc (void) {
  QPrinter * p = new QPrinter (QPrinter::HighResolution);
  p->setOutputFileName ("print.ps");
  QPrintDialog printDialog (p, this);
  if (printDialog.exec() == QDialog::Accepted) {
    editor->print (p);
  }
  delete p;
}

//! [1]
void MainWindow::setupEditor() {
  QFont font;
  font.setFamily ("Courier");
  font.setFixedPitch (true);
  font.setPointSize (14);
  font.setStretch (QFont::ExtraCondensed);

  editor = new CodeEditor;
  editor->setFont (font);
  editor->setUserTable (4, false);

  highlighter = new Highlighter (editor->document());

}
//! [1]

void MainWindow::setupFileMenu() {
  QMenu * fileMenu = new QMenu (tr ("&File"), this);
  menuBar()->addMenu (fileMenu);

  fileMenu->addAction (tr ("&New"),       this, SLOT (newFile()),  QKeySequence::New);
  fileMenu->addAction (tr ("&Open..."),   this, SLOT (openFile()), QKeySequence::Open);
  fileMenu->addAction (tr ("&Save..."),   this, SLOT (saveCurr()), QKeySequence::Save);
  fileMenu->addAction (tr ("Save as..."), this, SLOT (saveFile()));
  fileMenu->addAction (tr ("Export as html..."), this, SLOT (exptHtml()));
  fileMenu->addAction (tr ("Print Document..."), this, SLOT (printDoc()));

  fileMenu->addAction (tr ("E&xit"), this, SLOT (close()), QKeySequence::Quit);
}

void MainWindow::setupHelpMenu() {
  QMenu * helpMenu = new QMenu (tr ("&Help"), this);
  menuBar()->addMenu (helpMenu);

  helpMenu->addAction (tr ("&About"),    this, SLOT (about()));
  helpMenu->addAction (tr ("About &Qt"), qApp, SLOT (aboutQt()));

}
void MainWindow::dispErr (const QString & msg) {
  ui->statusbar->setPalette (errPalette);
  ui->statusbar->showMessage (msg, 5000);
}
void MainWindow::dispMsg (const QString & msg) {
  ui->statusbar->setPalette (msgPalette);
  ui->statusbar->showMessage (msg, 3000);
}
/*
void MainWindow::Progress (const char * text) {
  QString msg (QString::fromUtf8(text));
  dispMsg (msg);
}
*/

void MainWindow::setupCompiler() {
  cf.Common = 0U;
  cf.F.O = cf.F.L = 1U;
//cf.F.TGT = LLVMTypesMax;
  err = new DisplayError (this);
  LogFce = err->log;
  ui->Messages->setWidget (err);
  QAction * ac = ui->toolBar->addAction (QIcon(":build"), tr ("Compile"), this, SLOT (Compile()));
  ac->setShortcut (QKeySequence (Qt::Key_F8));
}
bool MainWindow::assertFileChange (const QString & msg) {
  if (!editor->document()->isModified()) return true;
  QMessageBox msgBox;
  msgBox.setText (QString::fromUtf8 ("Zdrojový soubor byl modifikován."));
  msgBox.setInformativeText (msg);
  msgBox.setStandardButtons (QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
  msgBox.setDefaultButton   (QMessageBox::Save);
  int ret = msgBox.exec();

  switch (ret) {
    case QMessageBox::Save:
      if (currentFileName.isEmpty()) return false;
      saveCurr ();
      break;
    case QMessageBox::Discard:
      break;
    case QMessageBox::Cancel:
      return false;
      break;
    default:
      break;
  }
  return true;
}
void MainWindow::Compile (void) {
  if (currentFileName.isEmpty()) return;
  err->clear();
  if (!assertFileChange(QString::fromUtf8("Close - bude přeložen starý soubor\n"
                                          "Cancel - nic se nepřekládá"))) return;
//const char * toCompile = currentFileName.toAscii(). data();
  const char * toCompile = currentFileName.toLatin1().data();
  int result = Simple (toCompile, cf.Common);
  if (result) dispErr (QString::fromUtf8 ("Překlad skončil s chybou - klikni na označení chyby"));
  
  assertDlnFile();
  viewer->AddFileWatchs(currdnlFileName);
}
void MainWindow::DisplayStr (const char * msg) {
  err->log (msg);
}
bool MainWindow::assertDllFile (void) {
  bool result = false;
  if (currentFileName.isEmpty()) {
    dispErr (QString::fromUtf8("Není otevřen žádný soubor"));
    return result;
  }
  const char * from = ".stp", * to = ".mac.so";
  int pos = currentFileName.lastIndexOf(from, -1, Qt::CaseInsensitive);
  if (pos < 0) {
    dispErr (QString::fromUtf8("Musíte mít otevřen nějaký .stp soubor"));
    return result;
  }
  currdnlFileName = currentFileName;
  currdnlFileName.replace(pos, 4, to);
  QFile dnlFile (currdnlFileName);
  if (!dnlFile.exists()) {
    dispErr (QString::fromUtf8("Musíte napřed přeložit .stp soubor"));
    return result;
  }
  result = true;
  //qDebug("DLL=%s", currdnlFileName.toAscii().data());
  return result;
}

bool MainWindow::assertDlnFile (void) {
  bool result = false;
  if (currentFileName.isEmpty()) {
    dispErr (QString::fromUtf8("Není otevřen žádný soubor"));
    return result;
  }
  const char * from = ".stp", * to = ".dnl";
  int pos = currentFileName.lastIndexOf(from, -1, Qt::CaseInsensitive);
  if (pos < 0) {
    dispErr (QString::fromUtf8("Musíte mít otevřen nějaký .stp soubor"));
    return result;
  }
  currdnlFileName = currentFileName;
  currdnlFileName.replace(pos, 4, to);
  QFile dnlFile (currdnlFileName);
  if (!dnlFile.exists()) {
    dispErr (QString::fromUtf8("Musíte napřed přeložit .stp soubor"));
    return result;
  }
  result = true;
  return result;
}
void MainWindow::LoadDnl (void) {
  switch (cf.F.TGT) {
    case MachineTypeCortexM0:
    case MachineTypeCortexM3:
    case MachineTypeCortexM4F:
      qDebug("Zatim neni podpora pro tuto architekturu");
      return;
    case MachineTypeLinux64:
      if (!assertDllFile()) return;
      if (simulace->isVisible()) simulace->close();
//    if (LoadLibPes (currdnlFileName.toAscii().constData())) return;
      if (LoadLibPes (currdnlFileName.toLatin1().constData())) return;
      simulace->StartSimulator();
      return;
    default:
      break;
  }
  if (!assertDlnFile()) return;
  if (editor->document()->isModified()) {
    QMessageBox msgBox;
    msgBox.setText (QString::fromUtf8 ("Zdrojový soubor byl modifikován."));
    msgBox.setInformativeText (QString::fromUtf8 ("Musíte ho uložit a znovu přeložit"));
    msgBox.setStandardButtons (QMessageBox::Save | QMessageBox::Cancel);
    msgBox.setDefaultButton   (QMessageBox::Save);
    int n = msgBox.exec();
    if (n == QMessageBox::Save) saveCurr();
    return;
  }
  QDateTime dd = QFileInfo(currdnlFileName).lastModified();
  QDateTime ds = QFileInfo(currentFileName).lastModified();
  if (dd < ds) {
    QMessageBox msgBox;
    msgBox.setText (QString::fromUtf8 ("Cílový *.dnl soubor je starší než zdrojový"));
    msgBox.setInformativeText (QString::fromUtf8 ("Musíte ho znovu přeložit"));
    msgBox.setStandardButtons (QMessageBox::Ok);
    msgBox.setDefaultButton   (QMessageBox::Ok);
    msgBox.exec();
    return;
  }
  qDebug ("Download: %s", currdnlFileName.toLatin1().data());
  //-return;
  viewer->WatchStart (false);
  err->clear();
  if (loader->Download (currdnlFileName.toLatin1().data())) {
    dispMsg("Download started.");
  }
}

void MainWindow::rcvMsg (const QString msg) {
  err->AddString(msg);
}

void MainWindow::DownloadEnd (void) {
  qDebug ("Download skoncil");
  bool res = loader->getResult();
  if (res) {
    dispMsg ("Download OK.");
    viewer->WatchStart (true);
  }
  else dispErr ("Download se nepovedl");
}
