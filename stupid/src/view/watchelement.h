#ifndef WATCHELEMENT_H
#define WATCHELEMENT_H

#include <QString>
#include <QMetaType>
#include "PesNet.h"
#include "inputdialog.h"

/** @class WatchElement
 *  @brief Bázová třída sledující jeden parametr zařízení.
 * 
 * Od toho jsou odvozeny všechny metody sledování.
 * */
class WatchElement {
  public:
    /**
     * @brief Základní konstruktor nemá parametry, vše se musí postupně nastavit.
     */
    WatchElement (unsigned b=16) : isFromFile (false) {
      val = 0; base = b; pes = 0; address = 0; input = 0; aliases = NULL;
    };
    /**
     * @brief Nastavení driveru a adresy prvku, to se dělá vždy najednou.
     * 
     * @return void
     */
    void setElement (CPesNet * driver, unsigned int addr) {
      pes = driver; address = addr;
    };
    /**
     * @brief Nastavení Widgetu pro vstup hodnoty.
     * 
     * @return void
     */
    virtual void setInput      (VirtualInput * i) {
      input = i;
    }
    /**
     * @brief Vrací načtenou hodnotu ze sítě, ale jen v odvozených metodách.
     * 
     * @return bool true, pokud je úspěch.
     */
    virtual bool getWatchValue (      unsigned int & value) {value=val; return true;};
    /**
     * @brief Nastaví hodnotu v síti, ale jen v odvozených metodách.
     * 
     * @return bool true, pokud je úspěch.
     */
    virtual bool setWatchValue (const unsigned int & value) {val=value; return true;};
    /**
     * @brief Pokud se hodnota na síti změnila, nastaví changed.
     * 
     * @param value   Zde se vrátí nová hodnota.
     * @param changed Vrátí true, pokud se změnila, jinak false.
     * @return bool true, pokud je úspěch.
     */
    virtual bool ValueChanged  (      unsigned int & value, bool & changed)=0;
    /**
     * @brief Převod na string - zde je jediné místo, kde to jde změnit třeba na hexadecimální základ.
     * 
     * Lze to přetížit v odvozených třídách, i zde je to tak použito.
     * 
     * @return QString&
     */
    virtual QString & StrValue (void);
     /** @brief Vrátí true, pokud je nastavena přezdívka.
     * Používá se třeba, pokud je byte použit jako skupina bitů.
     * @return bool
     */
    virtual bool HasAlias      (void)              const;
    /**
     * @brief Hledá přezdívku.
     * 
     * @param s Hledaný QString
     * @return bool true, pokud přezdívka obsahuje s.
     */
    virtual bool AliasContains (const QString & s) const;
    /**
     * @brief Nastaví přezdívky
     * 
     * @param dsc  Popisný řetězec asciii.
     * @param from číslo začátku
     * @param to   číslo konce (včetně)
     */
    virtual void SetAliases    (const char * dsc, int from, int to);
    /// Destruktor
    virtual ~WatchElement () {
      if (input) delete input; if (aliases) delete aliases;
    };
  public:
    CPesNet      * pes;         //!< udržuje ukazatel na ovladač.
    unsigned int   address;     //!< adresa sledováné proměnné
    unsigned int   val;         //!< hodnota sledováné proměnné
    QString        str;         //!< pomocná proměnná pro převod na string
    unsigned       base;
    
    VirtualInput * input;       //!< vstupní metoda (Widget)
    QStringList  * aliases;     //!< Přezdívky proměnné
    
    bool isFromFile;
};
/// Ukazatel na třídu WatchElement je použit jako QVariant ve Widgetu treeWidget,
/// respektive v jeho položkách QTreeWidgetItem.
Q_DECLARE_METATYPE(WatchElement*)

class WatchSTPWord : public WatchElement {
  public:
    WatchSTPWord ();
    bool getWatchValue (unsigned int & value);
    bool setWatchValue (const unsigned int & value);
    bool ValueChanged  (unsigned int & value, bool & changed);
};
class WatchSTPBit : public WatchElement {
  public:
    WatchSTPBit ();
    bool getWatchValue (unsigned int & value);
    bool setWatchValue (const unsigned int & value);
    bool ValueChanged  (unsigned int & value, bool & changed);
};
/** ***********************************************************************************/
class WatchXWord : public WatchElement {
  public:
    WatchXWord ();
    bool getWatchValue (unsigned int & value);
    bool setWatchValue (const unsigned int & value);
    bool ValueChanged  (unsigned int & value, bool & changed);
};

class WatchXByte : public WatchElement {
  public:
    WatchXByte();
    bool getWatchValue (unsigned int & value);
    bool setWatchValue (const unsigned int & value);
    bool ValueChanged  (unsigned int & value, bool & changed);
    /// WatchXByte má převod na string jinak - defakto binární.
    QString & StrValue (void);
};
#endif // WATCHELEMENT_H
