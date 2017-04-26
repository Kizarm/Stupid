#ifndef VIEWWINDOW_H
#define VIEWWINDOW_H

//#include <QtGui/QDialog>
#include <QDialog>
#include <QIcon>
#include "../posix/lib/Compiler.h"

class QSettings;
class WatchElement;
class Ui_ViewDialog;
class QTreeWidgetItem;
class QTableWidgetItem;
class WatchLoop;
class CPesNet;
class Ally;
class MainWindow;
class QAction;
class DnlVariables;
class DnlItem;

/**
 * @class  ViewDialog
 * @brief  Viewer.
 * 
 * Tvoří vlastní okno, zapouzdřuje vše co souvisí se zobrazením.
 * */

class ViewDialog : public QDialog {
  Q_OBJECT
  public:
    ViewDialog (MainWindow * parent = 0, Qt::WindowFlags flags = 0);
    void Progress (int n);
    bool WatchStart (bool on);
    MainWindow * GetParent (void) {return parent;}
    void RemoveFileWatchs  (void);
    void AddFileWatchs     (const QString & filename);
    void SetDnlItemPointer (DnlItem * ptr);
    void TargetChanged     (LLVMTypeMachine tgt);
    ~ViewDialog();
  public slots:
    void StartWatching (void);
    void TreeDouble (QTreeWidgetItem  * i, int n);
    void TreeSingle (QTreeWidgetItem  * i, int n);
    void TabSel     (QTableWidgetItem * i);
    void ChangeVal  (int i);
    void StartStop  (void);
    void Terminated (void);
    void Restart    (void);
    void FindVar    (void);
    void FindStation(bool);
    void SetStation (bool);
    WatchLoop * getLoop() {return loop;};
  protected:
    bool TestSettings   (void);
    bool AddActionNum   (int ord, int addr);
    bool ComInit        (void);
    void BuildTree      (void);
    void BuildInfo      (void);
    void GetRunState    (void);
    bool GetStationInfo (unsigned int station);
    QTreeWidgetItem * SearchVar (QTreeWidgetItem * e, const QString & s);
    void Expand (QTreeWidgetItem * e);
    bool ItemContains (const QTreeWidgetItem * item, const QString & find);
  private:
    MainWindow    * parent;
    DnlVariables  * dnlvar;
    QAction * Station[30];
    QIcon     mainico;
    QFont     msgFont;
    QPalette  msgPalette;
    QList<WatchElement *> list;
    QSettings     * setting;
    WatchLoop     * loop;
    Ui_ViewDialog * ui;
    CPesNet       * pes;
    Ally          * ally;
    QAction       * actionSelect_Station;
    
    bool     running;
};

#endif // MAINWINDOW_H
