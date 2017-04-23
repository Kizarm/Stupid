#include "watchelement.h"


QString & WatchElement::StrValue (void) {
  QString tmp;
  switch (base) {
    case 2:  tmp.setNum (val, 2); str = "0b" + tmp; break;
    case 8:  tmp.setNum (val, 8); str = "0"  + tmp; break;
    case 16: tmp.setNum (val,16); str = "0x" + tmp; break;
    default: str.setNum (val); break;
  }
  return str;
}

/*****************************************************/
WatchSTPWord::WatchSTPWord () : WatchElement () {
}
bool WatchSTPWord::getWatchValue (unsigned int & value) {
  if (!pes)               return false;
  if (pes->isDisabled())  return false;
  uint16_t result = pes->ReadSTPWord (address);
  if (!pes->isSuccsess()) return false;
  val = value = result;
  return true;
}
bool WatchSTPWord::setWatchValue (const unsigned int & value) {
  if (!pes)               return false;
  if (pes->isDisabled())  return false;
  uint16_t desired = (uint16_t) value;
  if (pes->WriteSTPWord (address, desired)) {
    val = value;
    return true;
  }
  return false;
}
bool WatchSTPWord::ValueChanged (unsigned int & value, bool & changed) {
  changed = false;
  if (!pes)               return false;
  if (pes->isDisabled())  return false;
  uint16_t result = pes->ReadSTPWord (address);
  if (!pes->isSuccsess()) return false;
  if (result != val) {
    val = value = result;
    changed = true;
  }
  return true;
}

/*****************************************************/
WatchSTPBit::WatchSTPBit () : WatchElement () {
}
bool WatchSTPBit::getWatchValue (unsigned int & value) {
  if (!pes)               return false;
  if (pes->isDisabled())  return false;
  uint8_t result = pes->ReadSTPBit (address);
  if (!pes->isSuccsess()) return false;
  val = value = result;
  return true;
}
bool WatchSTPBit::setWatchValue (const unsigned int & value) {
  if (!pes)               return false;
  if (pes->isDisabled())  return false;
  unsigned char desired;
  if (value) desired = 1;
  else       desired = 0;
  if (pes->WriteSTPBit (address, desired)) {
    val = value;
    return true;
  }
  return false;
}
bool WatchSTPBit::ValueChanged (unsigned int & value, bool & changed) {
  changed = false;
  if (!pes)               return false;
  if (pes->isDisabled())  return false;
  uint8_t result = pes->ReadSTPBit (address);
  if (!pes->isSuccsess()) return false;
  if (result != val) {
    val = value = result;
    changed = true;
  }
  return true;
}
/*****************************************************/
WatchXWord::WatchXWord() : WatchElement() {
}
bool WatchXWord::getWatchValue (unsigned int & value) {
  bool result = false;
  if (!pes)               return result;
  if (pes->isDisabled())  return result;
  uint16_t desired = 0;
  result = pes->ReadXWord (address, & desired);
  if (!result)            return result;
  val = value = desired;
  return result;
}
bool WatchXWord::setWatchValue (const unsigned int & value) {
  if (!pes)               return false;
  if (pes->isDisabled())  return false;
  uint16_t desired = (uint16_t) value;
  if (pes->WriteXWord (address, desired)) {
    val = value;
    return true;
  }
  return false;
}
bool WatchXWord::ValueChanged (unsigned int & value, bool & changed) {
  bool result = false;
  changed     = false;
  if (!pes)               return result;
  if (pes->isDisabled())  return result;
  uint16_t desired = 0;
  result = pes->ReadXWord (address, & desired);
  if (!result)            return result;
  if (desired != val) {
    val = value = desired;
    changed = true;
  }
  return true;
}
/*****************************************************/
WatchXByte::WatchXByte() : WatchElement() {
  aliases = NULL;
}
bool WatchXByte::getWatchValue (unsigned int & value) {
  bool result = false;
  if (!pes)               return result;
  if (pes->isDisabled())  return result;
  uint8_t desired = 0;
  desired = pes->ReadXByte (address);
  result  = pes->isSuccsess();
  if (!result)            return result;
  val = value = desired;
  return result;
}
bool WatchXByte::setWatchValue (const unsigned int & value) {
  if (!pes)               return false;
  if (pes->isDisabled())  return false;
  uint8_t desired = (uint8_t) value;
  if (pes->WriteXByte (address, desired)) {
    val = value;
    return true;
  }
  return false;
}
bool WatchXByte::ValueChanged (unsigned int & value, bool & changed) {
  bool result = false;
  changed     = false;
  if (!pes)               return result;
  if (pes->isDisabled())  return result;
  uint8_t desired = 0;
  desired = pes->ReadXByte (address);
  result  = pes->isSuccsess();
  if (!result)            return result;
  if (desired != val) {
    val = value = desired;
    changed = true;
  }
  return true;
}
QString & WatchXByte::StrValue (void) {
  char buffer [9];
  for (unsigned i=0; i<8; i++) {
    if (val & (1U << i)) buffer [i] = '1';
    else                 buffer [i] = '0';
  }
  buffer [8] = '\0';
  str = buffer;
  return str;
}
/*****************************************************/

bool WatchElement::HasAlias (void) const {
  if (aliases) return true;
  return false;
}

bool WatchElement::AliasContains (const QString & fs) const {
  bool result = false;
  if (!aliases)            return result;
  if ( aliases->isEmpty()) return result;
  int n = aliases->count();
  for (int i=0; i<n; i++) {
    QString ms = aliases->at(i);
    if (! ms.compare (fs, Qt::CaseInsensitive)) return true;
  }
  return result;
}
void WatchElement::SetAliases (const char * dsc, int from, int to) {
  if (!aliases) aliases = new QStringList;
  QString s;
  for (int i=from; i<to+1; i++) {
    s.sprintf ("%s%d", dsc, i);
    aliases->append (s);
  }
}
