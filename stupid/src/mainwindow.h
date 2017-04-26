#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "highlighter.h"
#include "codeeditor.h"
#include "DisplayError.h"
#include "ui_xicht.h"
#include "../posix/lib/Compiler.h"
#include "./pesim/pesim.h"

class ViewDialog;
class QSettings;
class CPesNet;
class CLoaderDlg;
class QComboBox;
//class SimDialog;

/**
 * @mainpage Sedit
 * 
 * @brief Pokus o multiplatformní nástroj pro práci s automaty Hypel.
 * 
 * Editor je koncipován jen pro práci s jedním .stp souborem. Ono to tak obvykle bývá.
 * 
 * 1. Překladač vychází z původních zdrojáků, odstraněny jsou závislosti na Windows.
 * Starý assembler a linker je zakomponován pomocí API do knihovny, stejně jako
 * překladač simple. Původně byl volán jako externí aplikace. Obojí je v separátním
 * adresáři posix, který je součástí tohoto stromu. API umožňuje vytvořit
 * aplikaci pro příkazovou řádku.
 * 
 * 2. Do editoru je zaintegrován i jednoduchý viewer dost podobný tomu původnímu.
 * To je uděláno s pomocí tříd @ref CPesNet a @ref CUSBPes (mírně upravených) od základu
 * znova v Qt, protože původní Windows verze pro mě byla nečitelná. Takže některé
 * věci můžou být problematické, je to šito horkou jehlou a jak se to postupně
 * rozrůstalo, stal se z toho stejný chaos jako to původní. Chtělo by to předělat.
 * 
 * 3. Loader je celkem převzat původní, není až tak složitý. API třída v adresáři
 * posix se liší od použité zde - ta je upravena pro Qt (např. dědí QThread),
 * je to tak jednodušší. Download je nastaven pro číslo stanice, použité v editovaném
 * .stp souboru.
 * 
 * Chybí:
 * - Dokumentace většiny tříd. A tedy i nějaký help.
 * - Nastavení stanice - baudrate a číslo, to dělat nebudu - mám jediný automat
 * a nechci ho zničit.
 * - Lepší možnosti editoru, např. vyhledávání. Ono je to vzato z příkladu, takže
 * asi nejsou ošetřeny všechny vylomeniny, které může uživatel dělat.
 * - Nejsou podporovány (ve vieweru) typy kromě WORD. To by bylo dost práce.
 * - A mnoho dalšího (Progress u Downloadu atd.)
 * 
 * Vcelku by to mohlo přežít i Qt5, ale určeno je to pro Qt4. Nicméně původní, plná
 * Windows verze je neudržovatelná, protože novější Visual Studio si s tím už neporadí.
 * Testováno na Ubuntu 12.04 a 14.04, Qt4.x, 64-bitová verze s libusb-dev z repozitáře.
 * 
 * Přidáno:
 * a. Překladač pro jiné architektury než 8052, založený na LLVM. Není úplně dokončen
 * a asi ani nebude, protože předávání parametru typu bit do funkcí a procedur nejde
 * na obecné architektuře jednoduše udělat. To však v zásadě není nutné používat.
 * 
 * b. Simulátor automatu. Protože je možné vyrobit ze *.stp zdrojáků nativní kód, není
 * nutné zatěžovat se simulací instrukčního kódu cílové architektury a lze použít
 * princip dlopen() a spuštění nativního kódu. Takže na druhé straně se musí jen
 * nasimulovat chování automatu (časovače, síť, ...). Síť je pouze nastíněna - umožňuje
 * pouze komunikaci pomocí síťových wordů, ale protože se používá UDP, lze spojit
 * simulované automaty na stejném segmentu ethernetu (tj. na různých počítačích).
 * Simulátor je jednak integrován do IDE ale i jako samostatná aplikace (stejné zdrojáky),
 * což se hodí pro zkoušení komunikace. Nelze k tomu připojit wiever.
 * 
 * @class MainWindow
 * @brief Hlavní okno programu.
 * 
 * Zapouzdřuje v sobě vše ostatní.
 * */

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow (QWidget * parent = 0);
    virtual ~MainWindow();
    QMenu     * GetMenuNet  (void) {return menuNet;};
    QSettings * GetSettings (void) {return setting;};
    CPesNet   * GetNetDriver(void) {return pes;};
    void SetNetDriver(CPesNet * p) {pes=p;};
    //void    Progress   (const char * text);
    void        DisplayStr  (const char * msg);

  public slots:
    void about();
    void newFile();
    void openFile (const QString & path = QString());
    void saveFile (const QString & path = QString());
    void exptHtml (const QString & path = QString());
    void rcvMsg   (const QString msg);
    void DownloadEnd (void);
    void printDoc (void);
    void saveCurr (void);
    void changeTarget (int);
    
    void LoadDnl  (void);
    void Compile  (void);
    //-void close    (void);
  protected:
    void TestSettings  (void);
    bool assertDlnFile (void);
    bool assertDllFile (void);
    void closeEvent (QCloseEvent * event);
    void Visibility    (void);

  private:
    QAction       * acWiew;
    QAction       * acLoad;
    CPesNet       * pes;
    Ui_MainWindow * ui;
    QComboBox     * psel;
    void setupEditor();
    void setupFileMenu();
    void setupHelpMenu();
    void setupCompiler();
    void setupTarget  ();
    void dispErr (const QString & msg);
    void dispMsg (const QString & msg);
    bool assertFileChange (const QString & msg);

    CodeEditor   * editor;
    Highlighter  * highlighter;
    DisplayError * err;
    ViewDialog   * viewer;
    CLoaderDlg   * loader;
    SimDialog    * simulace;
    QMenu        * menuNet;
    QSettings    * setting;
    
    QString  currentFileName;
    QString  currdnlFileName;
    QPalette msgPalette;
    QPalette errPalette;
    
    CompilerFlags cf;
};

#endif
