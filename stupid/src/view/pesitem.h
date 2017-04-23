#ifndef PESITEM_H
#define PESITEM_H

#include <QTreeWidgetItem>

class CPesNet;
class WatchElement;
class WatchXBit;
class WatchSTPWord;
class WatchXWord;

class XWord {
  public:
    XWord    (bool fromFile = false);
    void Set (QTreeWidgetItem * p, CPesNet * d, const char * dsc, unsigned addr, int no = -1);
    ~XWord   ();
  private:
    QTreeWidgetItem * item;
    WatchElement    * word;
};
class XByte {
  public:
    XByte    ();
    void Set (QTreeWidgetItem * p, CPesNet * d, const char * dsc, unsigned addr, int no = -1);
    ~XByte   ();
  private:
    QTreeWidgetItem * item;
    WatchElement    * byte;
};
class SBit {
  public:
    SBit     ();
    void Set (QTreeWidgetItem * p, CPesNet * d, const char * dsc, unsigned addr, int no = -1);
    ~SBit    ();
  private:
    QTreeWidgetItem * item;
    WatchElement    * bit;
};

/***********************************************************************************************/
class XWords {
  public:
    XWords (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc, const char * mark,
            unsigned addr, unsigned from, unsigned count);
    ~XWords ();
  private:
    QTreeWidgetItem * root;
    XWord           * words;
};
class XBytes {
  public:
    XBytes (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc, const char * mark,
            unsigned addr, unsigned from, unsigned count);
    ~XBytes ();
  private:
    QTreeWidgetItem * root;
    XByte           * bytes;
};

/***********************************************************************************************/
class System {
  public:
    System  (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc);
    ~System ();
  private:
    QTreeWidgetItem * root;
    SBit            * reset;
    SBit            * turbo;
    SBit            * eesav;
    SBit            * eeloa;
    XWord           * eewrc;
    XWord           * speed;
    XWord           * myadr;
    SBit            * slavm;
};
class Timer {
  public:
    Timer    ();
    void Set (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc, int no);
    ~Timer   ();
  private:
    QTreeWidgetItem * root;
    XWord           * tim;
    SBit            * ten;
    SBit            * tdm;
    SBit            * tpa;
    SBit            * toe;
    SBit            * tof;
    SBit            * clk;
};
class Timers {
  public:
    Timers  (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc, int count);
    ~Timers ();
  private:
    QTreeWidgetItem * root;
    Timer           * tim;
};
/***********************************************************************************************/
class XLocal {
  public:
    XLocal (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc);
    ~XLocal();
  private:
    QTreeWidgetItem * root;
    XWords * regs;
    XBytes * rels;
};
class XPublic {
  public:
    XPublic (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc);
    ~XPublic();
  private:
    QTreeWidgetItem * root;
    XWords * regs;
    XBytes * rels;
};
class Absolute {
  public:
    Absolute (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc);
    ~Absolute();
  private:
    QTreeWidgetItem * root;
    XLocal  * loc;
    XPublic * pub;
};
/***********************************************************************************************/
class Settings {
  public:
    Settings (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc);
    ~Settings();
  private:
    QTreeWidgetItem * root;
    XWord  * adcm;
    SBit   * cals;
    XWords * calb;
};
class Analog {
  public:
    Analog (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc);
    ~Analog();
  private:
    QTreeWidgetItem * root;
    XWords   * inps;
    XWords   * outs;
    Settings * sets;
};
class Digital {
  public:
    Digital (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc);
    ~Digital();
  private:
    QTreeWidgetItem * root;
    XBytes * inps;
    XBytes * outs;
};
class Peripheral {
  public:
    Peripheral (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc);
    ~Peripheral();
  private:
    QTreeWidgetItem * root;
    Analog  * anl;
    Digital * dig;
};
/***********************************************************************************************/
class TDisplay {
  public:
    TDisplay (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc);
    ~TDisplay ();
  private:
    QTreeWidgetItem * root;
    XWord * fmt;
    XWord * pos;
};
class Keyboard {
  public:
    Keyboard (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc);
    ~Keyboard();
  private:
    QTreeWidgetItem * root;
    XWord * kbcod;
    XWord * kbdel;
    XWord * kbrep;
    SBit  * kbrpe;
    SBit  * kbsnd;
};
class Terminal {
  public:
    Terminal (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc);
    ~Terminal();
  private:
    QTreeWidgetItem * root;
    TDisplay * dis;
    Keyboard * key;
};
/***********************************************************************************************/
class RealTime {
  public:
    RealTime (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc);
    ~RealTime();
  private:
    QTreeWidgetItem * root;
    XWord * sec;
    XWord * min;
    XWord * hod;
    XWord * day;
    XWord * mon;
    XWord * rok;
    XWord * wek;
    SBit  * hold;
    SBit  * clrs;
};
class NetVar {
  public:
    NetVar (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc);
    ~NetVar();
  private:
    QTreeWidgetItem * root;
    XWord * scod;
    XWord * sadr;
    XWord * madr;
    XWord * badr;
    XWord * sdat;
    SBit  * sbit;
};
/***********************************************************************************************/
class DnlItem;

class Ally {
  public:
    Ally  (QTreeWidget * p, CPesNet * d, int no = 0);
    void SetStationNumber (int no);
    ~Ally ();
    QTreeWidgetItem * getRoot (void) { return root; };
  private:
    QTreeWidgetItem * root;
    
    DnlItem    * dnl;
    System     * sys;
    Absolute   * abs;
    Peripheral * per;
    Timers     * tim;
    RealTime   * rtc;
    Terminal   * trm;
    NetVar     * net;
    
};

#endif // PESITEM_H
