#ifndef DNLITEM_H
#define DNLITEM_H
#include <QWidget>

enum VariableType {
  VariableTypeIBit  = 0,
  VariableTypeIWord,
  VariableTypeBit,
  VariableTypeWord,
  VariableArrayBit,
  VariableArrayWord
};
struct VariableDesc {
  unsigned     alen;  // delka pole
  unsigned     addr;
  unsigned     badr;
  VariableType type;
  QString      name;
  VariableDesc() {
    alen = 0;
    addr = 0;
    badr = 0;
    type = VariableTypeIBit;
  }
};
class DnlItem;

class DnlVariables {
  public:
    DnlVariables ();
    ~DnlVariables ();
    bool readFile (const QString & filename);
    void SetDnlItem (DnlItem * i) {item=i;}
    bool addTree  (void);
    void print (void);
  protected:
    bool parseLine (const char * line, const int len);
    void InsertOrReplace (const VariableDesc & v);
  private:
    DnlItem * item;
    QVector<VariableDesc> var; 
};

#include <QTreeWidgetItem>

class CPesNet;
class WatchElement;
class WatchXBit;
class WatchSTPWord;
class WatchXWord;

class XWord;

class DnlItem {
  public:
    DnlItem  (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc);
    bool addVariable  (const VariableDesc & dsc);
    void removeAllOld (void);
    ~DnlItem ();
  protected:
  private:
    QTreeWidgetItem * root;
    CPesNet         * driver;

    QVector<XWord*>   words;
};

#endif // DNLITEM_H
