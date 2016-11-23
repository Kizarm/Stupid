#include <stdio.h>
#include <unistd.h>
#include "Loader.h"
/*
#include "platform.h"
void cstring_test (void) {
    CString str;
    str.Format ("Chyba : stanice č.%d nebyla detekována!", 12345);
    AfxMessageBox (str, MB_OK|MB_ICONSTOP, NULL);
}
*/
/*
#include <string>
using namespace std;
#include "platform.h"
void name_test (void) {
  BYTE  dato[9];
  int i;

  string m_strFilename = "../test/Test.dnl";
  // Uložení názvu DNL souboru
  string dnl = m_strFilename;
  i = m_strFilename.rfind (PathFileSeparator);
  if (i >= 0) {
    dnl = "";
    i++;
    for (; i < (int) m_strFilename.size(); i++)
      dnl += m_strFilename[i];
  }
  dnl.erase (dnl.rfind ('.'), 4);
  printf ("filename=[%s]\n", dnl.data());

  for (i = 0; i < 8; i++)
    dato[i] = 0;
  for (i = 0; i < (int) dnl.size(); i++)
    dato[i] = dnl[i];

  for (i = 0; i < 8; i++) {
    printf ("%02X ", dato[i]);
  }
  printf ("\n");
}
*/
int main (void) {
  CLoaderDlg loader;
  const char * file = "../test/Test.dnl";
  loader.Download (file);
//loader.Verify   (file);
  return 0;
}
