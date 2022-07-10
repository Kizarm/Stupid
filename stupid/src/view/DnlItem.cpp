#include "pesitem.h"
#include "watchelement.h"
#include "inputdialog.h"
#include "DnlItem.h"
#include "viewwindow.h"
#include "mainwindow.h"
#include <QFile>

DnlVariables::DnlVariables() {
  var.clear();
}
DnlVariables::~DnlVariables() {

}
bool DnlVariables::readFile (const QString & filename) {
  if (filename.isEmpty())                       return false;
  QFile dnl (filename);
  if (!dnl.exists())                            return false;
  if (!dnl.open(QFile::ReadOnly | QFile::Text)) return false;
  const int max = 0x400;
  char buf [max];
  int readen;
  var.clear();
  while (true) {
    readen = dnl.readLine (buf, max);
    if (readen < 0) break;
    parseLine (buf, readen);
  }
  
  dnl.close();
  //print();
  return true;
}
void DnlVariables::print (void) {
  VariableDesc tmp;
  if (var.isEmpty()) {
    printf ("nejaka chybicka\n");
    return;
  }
  int i, n = var.count();
  for (i=0; i<n; i++) {
    tmp = var.at(i);
    char * name = tmp.name.toLatin1().data();
    printf ("%03X-%04X.%X[%2d]:%s\n", tmp.type, tmp.addr, tmp.badr, tmp.alen, name);
  }
}
void DnlVariables::InsertOrReplace (const VariableDesc & v) {
  VariableDesc tmp;
  int i, n = var.count();
  bool result = false;

  for (i=0; i<n; i++) {
    tmp = var.at(i);
    if (tmp.type != v.type) continue;
    if (tmp.addr != v.addr) continue;
    if (tmp.badr != v.badr) continue;
    result = true;
    break;
  }

  if (result) {
    VariableDesc va = v;
    va.name += ',';
    va.name.append(var.at(i).name);
    var.replace(i,va);
  } else {
    var.push_back (v);
  }
}
// promenne typu real a int zatim vynechame
bool DnlVariables::parseLine (const char * line, const int) {
  if (line[0] != '#') return false;
  VariableDesc tmp;
  const int max = 64;
  char name [max], type[max], number[max];
  unsigned vlen, vadr, badr;
  int n;
  n = sscanf (line, "#bit %d %s", &vadr, name);
  if (n == 2) {
    //printf ("B%d: %s \tcislo=%d\n", n, name, vadr);
    QString s(name);
    tmp.type = VariableTypeIBit;
    tmp.addr = vadr;
    tmp.name = s;
    InsertOrReplace(tmp);
    return true;
  }
  n = sscanf (line, "#word %d %s", &vadr, name);
  if (n == 2) {
    //printf ("B%d: %s \tcislo=%d\n", n, name, vadr);
    QString s(name);
    tmp.type = VariableTypeIWord;
    tmp.addr = vadr;
    tmp.name = s;
    InsertOrReplace(tmp);
    return true;
  }
  n = sscanf (line, "#VAR %s %s \t%s", number, name, type);
  if (n == 3) {
    //printf ("V%d: %s \tcislo=%s,\ttyp=%s\n", n, name, number, type);
    if (!strcmp(type, "WORD")) {
      n = sscanf(number,"%d", & vadr);
      if (n != 1) return false;
      tmp.type = VariableTypeWord;
      tmp.addr = vadr;
    } else if (!strcmp(type, "BIT")) {
      n = sscanf(number,"%d.%d", & vadr, & badr);
      if (n != 2) return false;
      tmp.type = VariableTypeBit;
      tmp.addr = vadr;
      tmp.badr = badr;
    } else {
      return false;
    }
    QString s(name);
    tmp.name = s;
    InsertOrReplace(tmp);
    return true;
  }
  n = sscanf (line, "#ARR %s %s \t[%d]%s", number, name, &vlen, type);
  if (n == 4) {
    //printf ("A%d: %s=%d \tcislo=%s,\ttyp=%s\n", n, name, vlen, number, type);
    if (!strcmp(type, "WORD")) {
      n = sscanf(number,"%d", & vadr);
      if (n != 1) return false;
      tmp.type = VariableArrayWord;
      tmp.addr = vadr;
    } else if (!strcmp(type, "BIT")) {
      n = sscanf(number,"%d.%d", & vadr, & badr);
      if (n != 2) return false;
      tmp.type = VariableArrayBit;
      tmp.addr = vadr;
      tmp.badr = badr;
    } else {
      return false;
    }
    tmp.alen = vlen;
    QString s(name);
    tmp.name = s;
    InsertOrReplace(tmp);
    return true;
  }
  return true;
}
bool DnlVariables::addTree (void) {
  if (var.isEmpty()) return false;
  if (!item)         return false;
  item->removeAllOld();
  /*
  int i, n = var.count();
  for (i=0; i<n; i++) {
    if (!item->addVariable(var.at(i))) return false;
  }
  */
  for (const VariableDesc & vd: var) {
    if (!item->addVariable(vd)) return false;
  }
  //print();
  return true;
}
/** ***************************************************/
DnlItem::DnlItem (QTreeWidgetItem * p, CPesNet * d, const char * ico, const char * dsc) {
  root   = new QTreeWidgetItem (p);
  driver = d;
  QString s (dsc);
  root->setText    (0, s);
  root->setToolTip (0, s);
  root->setIcon    (0, QIcon (ico));
  
  //-printf ("DnlItem::DnlItem\n");
  ViewDialog * dlg = driver->GetParent();
  if (!dlg) return;
  dlg->SetDnlItemPointer(this);
}

DnlItem::~DnlItem() {
  //-printf ("DnlItem::~DnlItem\n");
  removeAllOld();
}
bool DnlItem::addVariable (const VariableDesc & dsc) {
  XWord * nword;
  switch (dsc.type) {
    case VariableTypeWord:
      nword = new XWord(true);
      //-printf ("addVariable\n");
      nword->Set (root, driver, dsc.name.toLatin1().constData(), dsc.addr);
      words.push_back (nword);
      break;
    default: break;
  }
  return true;
}
void DnlItem::removeAllOld (void) {
  if (!words.isEmpty()) {
    int i, n = words.count();
    for (i=0; i<n; i++) {
      //-printf ("removeAllOld %d\n", i);
      XWord * old = words.at(i);
      delete old;
    }
    words.clear();
  }
}
