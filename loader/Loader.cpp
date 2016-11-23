// LoaderDlg.cpp : implementation file
//
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

#include "Loader.h"
#include "PesDefines.h"
#include "platform.h"
#include "USBPes.h"
#include <memory.h>
#include <vector>
using namespace std;

//#define TRACE printf
#define TRACE(...)

/////////////////////////////////////////////////////////////////////////////
// CLoaderDlg dialog

CLoaderDlg::CLoaderDlg (/*CWnd* pParent =NULL*/) {
  m_nBlok = NULL;
  m_nStAddr = 31;
  m_bRegistred = false;
  m_bSTOP = false;
  m_bAutoUnlock = false;
  m_bAutoLock = false;
  m_bOnlyVerifi = false;
  m_bPassword = false;
  m_nBlockSize = 128;
  m_nPageBegin = 8;
  m_bNoLock = false;
  m_PES = NULL;
  m_bVerify = false;
}

bool CLoaderDlg::Verify (const char * filename) {
  m_strFilename = filename;
  bool result = false;
  do {
    if (!ReadDNLFile()) break;
    if (!InitPES ())    break;
    if (!GetPESInfo())  break;
    if (!Verifi  (m_nStartAddress, m_nCodeLen)) break;
    Status ("VERIFY OK.");
    if (!Reboot())      break;
    result = true;
  } while (0);
  m_PES->COM_Close();
  return result;
}
bool CLoaderDlg::Download (const char * filename) {
  m_strFilename = filename;
  bool result = false;
  do {
    if (!ReadDNLFile()) break;
    if (!InitPES ())    break;
    if (!GetPESInfo())  break;
    if (!Download  ())  break;
    if (!Reboot())      break;
    result = true;
  } while (0);
  m_PES->COM_Close();
  return result;
}

bool CLoaderDlg::GetStatus() {
  BYTE  Rep;

  Rep = NUM_REPEATS;
  // ziskej status
  do {
    Rep--;
    if (m_PES->ReliableSend (Cm_GetStatus)) {
      break;
    }
  } while (Rep != 0);

  if (Rep == 0) {
    AfxMessageBox ("Z automatu se nepodařilo vyčíst informace o kontrolním součtu!\nUjistěte se, že došlo ke kotrektnímu downloadu.", MB_OK|MB_ICONWARNING, NULL);
    return false;
  }
  // kontrolni soucet RAM
  if ( (m_PES->IOData[0] & 0x10) != 0) {
    AfxMessageBox ("Automat vrací chybu neplatného kontrolního součetu programu.\nPravděpodobně došlo k chybě při downloadu!", MB_OK|MB_ICONWARNING, NULL);
    return false;
  }

  return true;
}

bool CLoaderDlg::Reboot() {

  BYTE  RCnt;
  bool  ret;
  long int  wd;

  m_PES->NetBreak();

  m_PES->UserWD (&wd);
  do {
    RCnt = NUM_REPEATS;
    do {
      RCnt--;
      ret = m_PES->SendCmd (Cm_GetStatus);
      if (RCnt == 0)
        return false;
    } while (!ret);

    if (!m_PES->UserHaf (wd, 300)) {
      m_PES->SendCmd (Cm_GetIOBuf);
      break;
    }
  } while ( (m_PES->IOData[0] & ST_INBUSY) != 0);

  return m_PES->SendCmd (Cm_IO_Reboot);
}

void CLoaderDlg::Status (const char * pszFormat, ...) {
  char buf[1024];
  va_list arglist;
  va_start (arglist, pszFormat);
  vsprintf (buf, pszFormat, arglist);
  va_end (arglist);

  /*
  m_cInfo.AddString (buf);
  m_cInfo.UpdateWindow();
  m_cInfo.SetCurSel (m_cInfo.GetCurSel() + 1);
  */
  //! TODO: neco s tim udelej
  printf (SGREEN"%s"SDEFC"\n", buf);
}

bool CLoaderDlg::ReadDNLFile() {
  FILE * fDnl;
//CStringArray  DnlLines;
  vector<string> DnlLines;
  char  Radka[1000], nPocetBytu, nSoucet, j;
  unsigned int  nCisloRadky = 0, i = 0;//, nVelikostPole = 0;
  unsigned int  nSuma, nAdresa, k = 0 ;
  BYTE  nKod[41000];
  string pom;

  if ( (fDnl = fopen (m_strFilename.data(), "r")) == NULL) {
    ReportInfo ("File could not be opened !\n");
    AfxMessageBox ("Chyba : Nelze otevřít *.dnl soubor!",
                   MB_OK|MB_ICONSTOP, NULL);
    return  false;
  }
  // jednotlive radky souboru uloz do kolekce retezcu
  fseek (fDnl, 0, SEEK_SET);
  while (fgets (Radka, 1000, fDnl) != NULL) {
    // pokud to je radka s programem, uloz ji do seznamu
    if (Radka[0] == ':') {
      DnlLines.resize (nCisloRadky + 1/*, strlen (Radka) + 1*/);
      DnlLines.at (nCisloRadky) = Radka;
      nCisloRadky++;
    }
    if (strstr (Radka, "#Station") != NULL) {
      pom = Radka;
      pom.erase (0, 9);
      m_nStAddr = atoi (pom.data());
    }
    if (strstr (Radka, "#DataBegin") != NULL) {
      pom = Radka;
      pom.erase (0, 11);
      m_nDataBegin = atoi (pom.data());
    }
    if (strstr (Radka, "#DataSize") != NULL) {
      pom = Radka;
      pom.erase (0, 11);
      m_nDataSize = atoi (pom.data());
    }
  }
  fclose (fDnl);

  char p[5];

  for (i = 0; i < nCisloRadky; i++) {
    // nactu pocet bytu na radce
    p[0] = DnlLines[i][1];
    p[1] = DnlLines[i][2];
    p[2] = '\0';
    nPocetBytu = (char) strtoul (p, NULL, 16) * 2;
    // ted adresu
    p[0] = DnlLines[i][3];
    p[1] = DnlLines[i][4];
    p[2] = DnlLines[i][5];
    p[3] = DnlLines[i][6];
    p[4] = '\0';
    nAdresa = (unsigned int) strtoul (p, NULL, 16);
    if (i == 0) {
      m_nStartAddress = nAdresa;
    }
    // ted prectu kontrolni soucet
    p[0] = DnlLines[i][DnlLines[i].size()-3];
    p[1] = DnlLines[i][DnlLines[i].size()-2];
    p[2] = '\0';
    nSoucet = (unsigned int) strtoul (p, NULL, 16);
    nSuma = nSoucet + nAdresa + nPocetBytu/2;
    for (j = 0; j < nPocetBytu; j += 2) {
      p[0] = DnlLines[i][j+9];
      p[1] = DnlLines[i][j+10];
      p[2] = '\0';
      nKod[k] = (BYTE) strtoul (p, NULL, 16);
      nSuma += nKod[k];
      k++;
    }
  }

  m_nBlok = new BYTE[k];
  for (i = 0; i < k; i++)
    m_nBlok[i] = nKod[i];
  m_nCodeLen = k;
  ReportInfo ("Otevren soubor \'%s\'\n", m_strFilename.data());
  /* debug
  printf ("m_nStAddr=%d, m_nDataBegin=0x%04lX, m_nDataSize=%ld, m_nStartAddress=0x%04X\n",
          m_nStAddr, m_nDataBegin, m_nDataSize, m_nStartAddress);
  printf ("m_nCodeLen=%ld\n", m_nCodeLen);
  for (i=0,k=0; i<m_nCodeLen; i++) {
    printf ("%02X", m_nBlok[i]); k++;
    if (k==16) {
      printf ("\n"); k=0;
    }
  }
  printf ("\n");
  */
  return true;
}

bool CLoaderDlg::InitPES() {
  string   err;

  m_PES = new CUSBPes();
  m_PES->COM_Init (456786);
  m_PES->ComPort = 0;
  m_PES->SelectStation(m_nStAddr);
  if (!m_PES->Success) {
    /*! seriovy prevodnik nebudeme podporovat
    SAFE_DELETE (m_PES);
    m_PES = new CSerialPes();
    m_PES->m_n9600Wait = m_n9600Wait;
    m_PES->m_n19200Wait = m_n19200Wait;
    m_PES->m_n57600Wait = m_n57600Wait;
    m_PES->ComPort = m_nCom;
    Status ("Inicializuji COM%d", m_nCom);
    m_PES->COM_Init (456786);
    if (!m_PES->Success) {
      m_PES->COM_Close();
      m_PES->GetErrorString (err.GetBuffer (0));
      err.UnlockBuffer();
      AfxMessageBox (err, MB_OK|MB_ICONSTOP, NULL);

      SAFE_DELETE (m_PES);
      return false;
    }
    */
  } else {
    Status ("Inicializuji USB prevodnik");
  }

  Status ("Detekuji prenosovou rychlost");

  m_nBdRate = m_PES->BaudScan();
  if (!m_PES->Success) {
    ReportInfo ("Neznama chyba - BaudScan\n");
    /*! ???
    CScanDlg  dlg;
    dlg.m_PES = m_PES;
    dlg.m_mode = 1;
    if (dlg.DoModal() != 5)
      return false;
    m_nBdRate = dlg.m_nBaudRate;
    m_PES->SetBaudRate (m_nBdRate);
    */
  }

//  ::Sleep(1000);
  Status ("     detekovana rychlost %dBd", m_nBdRate);
  if (m_nStAddr > 30) { // jestlize je neplatne cislo stanice
    // detekuj aktivni stanice
    Status ("Detekuji stanice v siti");
    ReportInfo ("Detekuji stanice\n");

    m_PES->NetScan ();
    ReportInfo ("Detekovano %d stanic(e)\n", m_PES->StationNum);

    if (m_PES->StationNum > 1) {
      /*! Station zda se byt nanic, ale nechame, nicemu nevadi */
      Station = new StationDescriptor[m_PES->StationNum];
      //CSelStDlg dlg2;
      BYTE  i;
      string text;

      for (i = 0; i < m_PES->StationNum; i++) {
        BYTE  UserString[21],Dato;
        char  Jmeno[13];

        Station[i].stanice = m_PES->StationList[i];

        m_PES->SelectStation (Station[i].stanice);
        m_PES->NetBreak();
        if (!m_PES->ReadEEBlock ( (unsigned char *) Jmeno, 30, 8)) {
          delete [] Station;
          return false;
        }
        Jmeno[8] = 0;
        strcat (Jmeno,".DNL");
        Station[i].dnlname = Jmeno;

        // zjisti typ automatu
        if (!m_PES->ReadEEByte (EE_PIOTyp, Dato)) {
          delete [] Station;
          return false;
        }
        for (int j = 0; j <= 9; j++)
          m_PES->IOData[j] = 0;
        m_PES->IOData[0] = Dato;

        if (!m_PES->ReliableSend (Cm_GetPIOName)) {
          delete [] Station;
          return false;
        }
        int j;
        if (m_PES->IOData[0] == 0) {
          for (j = 0; j < 6; j++)
            UserString[j] = '0';
          UserString[j++] = '\0';
        } else {
          for (j = 0; j <= 5; j++)
            UserString[j] = m_PES->IOData[j];
          UserString[j++] = '\0';
        }
        Station[i].typ = (char*) UserString;

      }
/*
      dlg2.m_nPocet = m_PES->StationNum;
      dlg2.Station = Station;

      if (dlg2.DoModal() == IDCANCEL) {
        delete [] Station;
        return false;
      }
      m_nStAddr = m_PES->StationList[dlg2.m_nSel];
*/
      delete [] Station;
    } else {
      if (m_PES->StationNum == 0) {
        AfxMessageBox ("Chyba : v síti nebyla detekována žádná stanice!", MB_OK|MB_ICONSTOP, NULL);
        return false;
      }
      m_nStAddr = m_PES->StationList[0];
    }
  }

  m_PES->NetBreak();
  m_PES->NetBreak();

  m_PES->SelectStation (m_nStAddr); // vyber stanici

  m_PES->SendCmd (Cm_GetACK);
  if (m_PES->COM_Error && (m_PES->COM_Error != ERR_TIMEOUTRX)) {
    m_PES->SendCmd (Cm_GetACK);
  }

  if (m_PES->IOData[0] != 0x5A) {
    CString str;
    str.Format ("Chyba : stanice č.%d nebyla detekována!", m_nStAddr);
    AfxMessageBox (str, MB_OK|MB_ICONSTOP, NULL);
    return false;
  }

  Status ("Vybrana stanice číslo %d", m_nStAddr);

  return true;
}
/*
void CLoaderDlg::OnOK() {
  if (m_nBlok != NULL) {
    delete [] m_nBlok;
    m_nBlok = NULL;
  }
  if (m_PES != NULL) {
    m_PES->ReliableSend (Cm_ClrError);
    m_PES->FastToken();
    m_PES->COM_Close();
    SAFE_DELETE (m_PES);
  }

  CDialog::OnOK();
}
*/
bool CLoaderDlg::GetPESInfo() {
  unsigned long nCodeMem=0, nDataMem, nDataMemBegin;
  BYTE  pom;
  bool    fseries,err;
  char  rep;

  m_PES->NetBreak();

  /* Programova pamet */
  rep = 10;
  while (rep > 0) {
    err = false;
    if (!m_PES->ReadEEByte (EE_CodeMemCfg, pom)) {
      AfxMessageBox ("Chyba : Nelze číst z EEPROM", MB_OK|MB_ICONSTOP, NULL);
      return false;
    }
    fseries = false;
    switch (pom) {
      case  2 :
        nCodeMem = 8192  -1024;
        m_nBlockSize = 128;
        m_nPageBegin = 8;
        m_nVirtualPageStart = 0x0b;
        break;
      case  3 :
        nCodeMem = 32768 -1024;
        m_nBlockSize = 128;
        m_nPageBegin = 8;
        m_nVirtualPageStart = 0x0b;
        break;
      case  6 :
        fseries = true;
        nCodeMem = 40959;
        m_nBlockSize = 256;
        m_nPageBegin = m_nStartAddress / 256;
        m_nVirtualPageStart = 0x0;
        m_bNoLock = true;
        break;
      default:
        err = true;
//      AfxMessageBox( "Chyba : Tato paměťová konfigurace není podporována!", MB_OK|MB_ICONSTOP, NULL);
//      return false;
    }
    --rep;
  }
  if (err) {
    AfxMessageBox ("Chyba : Tato paměťová konfigurace není podporována!", MB_OK|MB_ICONSTOP, NULL);
    return false;
  }
  /* Datova pamet */
  if (!m_PES->ReadEEByte (EE_DataMemCfg, pom)) {
    AfxMessageBox ("Chyba : Nelze číst z EEPROM", MB_OK|MB_ICONSTOP, NULL);
    return false;
  }
  switch (pom) {
    case  1 :
    case  3 :
      nDataMem = 8192;
      nDataMemBegin = 4096;
      break;
    case  2 :
    case  6 :
    case  4 :
      nDataMem = 32768;
      nDataMemBegin = 4096;
      break;
    case  7 :
      nDataMem = 0x500;//2048;
      nDataMemBegin = 0x400;
      break;
    case  8 :
    case  9 :
    case  10  :
      nDataMem = 0xFFFF;
      nDataMemBegin = 4096;
      break;
    default:
      AfxMessageBox ("Chyba : Tato paměťová konfigurace není podporována!", MB_OK|MB_ICONSTOP, NULL);
      return false;
  }

  Status ("-------------------------------------");
  Status ("Maximalni delka programu \t%ld", nCodeMem);
  Status ("Delka aktualniho programu \t%ld", m_nCodeLen);
  Status ("Vyuziti programove pameti \t%ld%%", m_nCodeLen*100/nCodeMem);
  Status ("Dostupna datova pamet \t\t%ld", nDataMem);
  Status ("Vyuzita datova pamet \t\t%ld", m_nDataSize);
  Status ("Vyuziti datove pameti \t\t%ld%%", (m_nDataSize) ? (m_nDataSize*100/ (nDataMem-nDataMemBegin)) : 0);
  Status ("-------------------------------------");

  // Test zacatku kodove pameti
  if (m_nStartAddress == 0x8400 && fseries) {
    CString str;
    str.Format ("Chyba : Program neni kompilován pro automat série \"F\"!\
			\nRada : Ujistěte se, že máte správně nastavenu konfiguraci automatu ve zdrojovém textu.");
    AfxMessageBox (str, MB_OK|MB_ICONSTOP, NULL);
    return false;
  }
  if (m_nStartAddress == 0x4000 && !fseries) {
    CString str;
    str.Format ("Chyba : Program neni kompilován pro tento typ automatu, ale pro automat série \"F\"!\
			\nRada : Ujistěte se, že máte správně nastavenu konfiguraci automatu ve zdrojovém textu.");
    AfxMessageBox (str, MB_OK|MB_ICONSTOP, NULL);
    return false;
  }

  // Test velikosti obsazene kodove pameti
  if (nCodeMem < m_nCodeLen) {
    AfxMessageBox ("Chyba : Program je příliš dlouhý!", MB_OK|MB_ICONSTOP, NULL);
    return false;
  }


  // Test zacatku datove pameti
  if (nDataMemBegin != m_nDataBegin) {
    CString str;
    str.Format ("Chyba : Nesouhlasí nastavení začátku datové paměti!\
			\nRada : Ujistěte se, že máte správně nastavenu konfiguraci automatu ve zdrojovém textu.");
    AfxMessageBox (str, MB_OK|MB_ICONSTOP, NULL);
    return false;
  }

  // Test velikosti obsazene datove pameti
  if ( (nDataMem < (m_nDataBegin + m_nDataSize)) && m_nDataSize != 0) {
    AfxMessageBox ("Chyba : Program obsahuje příliš mnoho deklarovaných proměnných!", MB_OK|MB_ICONSTOP, NULL);
    return false;
  }
  return true;
}

bool CLoaderDlg::Download () {
  unsigned int  i;
  unsigned short  DnlSum = 0;
  char      rep = 0;
  //char      rep_down = 0;
  //BYTE      ErrCnt=0;
  BYTE      p;
  //bool      ok = false;
  bool      lock;

  // zastavim automat
  m_PES->SendCmd (Cm_IO_StupidStop);
  PlatformSleep (50);

  // vypnu obsluhu ovladacu
  m_PES->SendCmd (Cm_IO_StupidStep);

  // precti z automatu, stav uzamceni programove pameni
  //
  ReportInfo ("ReadLock\n");
  if (!m_PES->ReadEEByte (27, p)) {
    AfxMessageBox ("Chyba : Nelze číst z EEPROM", MB_OK|MB_ICONSTOP, NULL);
    return false;
  }

  switch (p) {
    case  0xaa:
      lock = true;
      break; // automat uzamcen
    case  0x55:
      lock = false;
      break;// jinak odemcen
    default   :
      lock = true;    // ...
  }
  if (lock) {   //! TODO
    Status ("Programova pamet je uzamcena");
    return false;
  }
/*
  if (lock) {
    Status ("Programova pamet je uzamcena");
    // pokud je pamet chranena heslem
    if (IsPassword()) {
      Status ("Automat je blokovan heslem");
      // zjistim ho
      CString entredpsw,psw;
      if (!ReadPassword (psw)) return false;

      // pokud nebylo heslo zadano z voleb Vieweru
      if (!m_bPassword) {
        // zeptam se uzivatele
        CGetPasswordDlg dlg;
        if (dlg.DoModal() == IDOK) {
          entredpsw = dlg.m_csPassword;
          entredpsw.MakeUpper();
        } else {
          return false;
        }
      } else {
        entredpsw = m_csPassword;
      }
      // otestuju ho
      if (! (entredpsw.GetLength() == 5 && entredpsw[0] == 'A' && entredpsw[1] == 'D' &&
             entredpsw[2] == 'E' && entredpsw[3] == 'L' &&
             entredpsw[4] == 'A') &&
          entredpsw != psw) {
        // pokud nesedi ==> konec
        AfxMessageBox ("Chyba : Chybné heslo!", MB_OK|MB_ICONSTOP, NULL);
        return false;
      }
    }

    // pokud neni nastaveo automatické odemknuti
    if (!m_bAutoUnlock && !m_bNoLock) {
      // zeptej se jestli se ma automat odemknout
      if (AfxMessageBox ("Programová paměť je uzamčena!\nPřejete si pokračovat?", MB_ICONSTOP|MB_YESNO) == IDNO) {
        // pokud ne ukonci to
        return false;
      }
    }

    // ted se ho pokus odemknout
    if (!Lock (false)) {
      return false;
    }

    Status ("Programova pamet odemcena");
  }
*/
  // Download
  //
  Status ("Download zacal");
  // ReportInfo ("DOWNLOAD ... start\n");

  //j = 0;
  //k = 0;

  // Vypocti kontolni soucet --> prosty soucet bytu
  // do bezznamenkove 2B promene(preteka pres 65535 -> 0)
  for (i = 0; i < m_nCodeLen; i++) {
    DnlSum += m_nBlok[i];
  }

  i = 0;
  //m_cDownload.SetRange (0, m_nCodeLen);

  BYTE * pBuffer = m_nBlok;
  BYTE  bError;
  CString csMessage;

  rep = 5;

  //CTime time1 = CTime::GetCurrentTime();

  //
  // Zacatek vlastniho downloadu
  //
  TRACE ("---------------------------------------------------------\n");
  TRACE ("DOWNLOAD - Soubor \"%s\", Velikost %ldB\n", m_strFilename.data(), m_nCodeLen);
  TRACE ("---------------------------------------------------------\n");
  m_PES->NetBreak();
  while (i < m_nCodeLen) {
    int size;

    //QuitAndPump();

    // pokud uzivatel klepne na tlacitko STOP, tak to cele ukonci
    if (m_bSTOP) {
      return false;
    }

    // nastaveni velikosti bloku, ktery se posle do RAM
    if (i + m_nBlockSize < m_nCodeLen) {
      size = m_nBlockSize;
    } else {
      size = m_nCodeLen - i;
    }

    // zapis bloku do RAM
    TRACE ("Zapis stranky %d o velikosti %d do RAM\n", (i / m_nBlockSize), size);
    bError = m_PES->DownloadBlock (m_nVirtualPageStart*256, size, pBuffer);
    if (bError != 16) { // chyba
      TRACE ("\tERROR\n");
      if (rep--) {
        continue;
      } else {
        csMessage.Format ("Chyba : Nepovedl se zapis %dB bloku do RAM(chyba %d)!", size, bError);
        AfxMessageBox (csMessage, MB_OK|MB_ICONSTOP, NULL);
        return false;
      }
    }

    // presun posledni stranky do kodove pameti
    TRACE ("Presun stranky %d do CODE\n", (i / m_nBlockSize));
    bError = m_PES->WriteEEByte (0xFF, m_nPageBegin + i/m_nBlockSize);
    if (bError != 0x5A) {
      if (rep--) {
        continue;
      } else {
        csMessage.Format ("Chyba : Nelze zapsat do kódové paměti(chyba %d)!", bError);
        AfxMessageBox (csMessage, MB_OK|MB_ICONSTOP, NULL);
        return false;
      }
    }

    // verifikace zapsaneho bloku
//    if (m_PES->ComPort != 0)
    {
      TRACE ("Verifikace kodove stranky c.%d\n", (i / m_nBlockSize));
      if (!Verifi (m_nStartAddress + i, size)) {
        TRACE ("\tERROR\n");
        if (rep--) {
          continue;
        } else {
          AfxMessageBox ("Chyba : Chybna verifikace!", MB_OK|MB_ICONSTOP, NULL);
          return false;
        }
      }
    }
    i += size;
    pBuffer += size;
    rep = 5;
    // nastav ukazatel prubehu downloadu
    // m_cDownload.SetPos (i);
  }
/*
  CTime time2 = CTime::GetCurrentTime();
  int   ts = time1.GetHour() * 3600 + time1.GetMinute() * 60 + time1.GetSecond();
  int   te = time2.GetHour() * 3600 + time2.GetMinute() * 60 + time2.GetSecond();
  m_nDownloadTime = te - ts;
*/
  //Verifi(m_nStartAddress, m_nCodeLen);

  // Zapis do automatu delku kodu
  if (!m_PES->WriteEEByte (EE_SizeLo, (BYTE) (m_nCodeLen%256))) {
    ReportInfo ("DOWNLOAD chyba pri zapisu delky kodu1\n");
    AfxMessageBox ("Chyba : Nelze zapsat do EEPROM", MB_OK|MB_ICONSTOP, NULL);
    return false;
  }
  if (!m_PES->WriteEEByte (EE_SizeHi, (BYTE) (m_nCodeLen/256))) {
    ReportInfo ("DOWNLOAD chyba pri zapisu delky kodu2\n");
    AfxMessageBox ("Chyba : Nelze zapsat do EEPROM", MB_OK|MB_ICONSTOP, NULL);
    return false;
  }

  // A checksum
  if (!m_PES->WriteEEByte (EE_CheckLo, (BYTE) (DnlSum%256))) {
    ReportInfo ("DOWNLOAD chyba pri zapisu dnlsum1\n");
    AfxMessageBox ("Chyba : Nelze zapsat do EEPROM", MB_OK|MB_ICONSTOP, NULL);
    return false;
  }
  if (!m_PES->WriteEEByte (EE_CheckHi, (BYTE) (DnlSum/256))) {
    ReportInfo ("DOWNLOAD chyba pri zapisu dnlsum2\n");
    AfxMessageBox ("Chyba : Nelze zapsat do EEPROM", MB_OK|MB_ICONSTOP, NULL);
    return false;
  }

  Status ("     download probehl");
  //
  // Konec downloadu

  // nastaveni autorun-u
  if (!SetAutoRun()) {
    //OnCancel();
    return false;
  }

  // Uloz informace o poslednim downloadu - cas, poradi,...
  Status ("Ukladam nazev souboru");
  if (!SaveInfo()) {
    //OnCancel();
    return false;
  }
  Status ("     ulozen");

  // Odemkni/Zamkni automat
  //
  ReportInfo ("Zamykam/odemykam\n");
  if (lock || m_bAutoLock) {
    if (!Lock (true)) {
      ReportInfo ("Zamykam/odemykam ... chyba\n");
      return false;
    }
    Status ("Programova pamet byla uzamcena");
    ReportInfo ("Zamknuto\n");
  }
  ReportInfo ("DOWNLOAD ... OK\n");
  return true;
}

bool CLoaderDlg::SetAutoRun() {
  BYTE  dato;

  if (!m_PES->ReadEEByte (EE_RunMode, dato)) {
    AfxMessageBox ("Chyba : Nelze číst z EEPROM", MB_OK|MB_ICONSTOP, NULL);
    return false;
  }

  dato |= 0x10;

  if (!m_PES->WriteEEByte (EE_RunMode, dato)) {
    AfxMessageBox ("Chyba : Nelze zapsat do EEPROM", MB_OK|MB_ICONSTOP, NULL);
    return false;
  }
  Status ("AUTORUN mod nastaven");

  return true;
}
/*
void CLoaderDlg::OnCancel() {
  if (m_nBlok != NULL) {
    delete [] m_nBlok;
    m_nBlok = NULL;
  }
  if (m_PES != NULL) {
    m_PES->ReliableSend (Cm_ClrError);
    m_PES->FastToken();
    m_PES->COM_Close();
    SAFE_DELETE (m_PES);
  }

  CDialog::OnCancel();
}
*/
bool CLoaderDlg::Verifi() {
  unsigned int     i,Offset = 0x8400;
  unsigned char    ErrCnt=0,XBuffer[0x8000];

  ReportInfo ("Verify() ... START\n");
  m_PES->IOData[0] = Offset / 256;
  m_PES->IOData[1] = Offset % 256;
  if (!m_PES->ReliableSend (Cm_SetXPtr)) {
    ReportInfo ("Verify() ... SetXPtr\n");
    return false;
  }
  i = 0;

  do {
    m_PES->SendCmd (Cm_ReadXPB);
    XBuffer[i] = m_PES->IOData[0];
    if (m_PES->COM_Error) {
      if (ErrCnt > NUM_REPEATS) {
        ReportInfo ("Verify() ... ReadXPR - moc opakovani\n");
        return false;
      }
      m_PES->SendCmd (Cm_ClrError);
      ErrCnt++;
      m_PES->IOData[0] = (Offset + i) / 256;
      m_PES->IOData[1] = (Offset + i) % 256;
      if (! m_PES->ReliableSend (Cm_SetXPtr)) {
        ReportInfo ("Verify() ... SetXPtr - v opakovani\n");
        return false;
      }
    } else {
      //ReportInfo("\n%d,%d\n",XBuffer[i], m_nBlok[i]);
      if (XBuffer[i] != m_nBlok[i]) {
        ReportInfo ("Verify() ... Rozdilny Byt\n");
        return false;
      }
      i++;
      ErrCnt = 0;
      //m_cVerify.SetPos( i);
    }
  } while (i < m_nCodeLen);

  ReportInfo ("Verify() ... OK\n");

  return true;
}

bool CLoaderDlg::Verifi (unsigned int Offset, unsigned int Length) {
  int i, j;
  BYTE * verbuff = NULL;
  bool retval = false;

  i = Offset - m_nStartAddress;
  verbuff = new BYTE[Length];
  memset (verbuff, 0x00, Length);

  if (m_PES->ReadCodeBlock (verbuff, Offset, Length)) {
    retval = true;
    for (j = 0; j < (int) Length; j++) {
      if (verbuff[j] != m_nBlok[i + j]) {
        TRACE ("Testuji [%d]=%d == %d\n", i + j, m_nBlok[i + j], verbuff[j]);
        TRACE ("ERROR\n");
        retval = false;
        break;
      }
    }
  }

  delete [] verbuff;
  return retval;
}


bool CLoaderDlg::SaveInfo() {
  unsigned short  DownCount = 0;
  BYTE  dato[9];
  int i;

  /* Inkrementece počtu downloadů */
  ReportInfo ("SaveInfo ... Inkrementace poctu downloadu\n");
  if (!m_PES->ReadEEBlock (dato, 28, 2)) {
    AfxMessageBox ("Chyba : Nelze číst z EEPROM", MB_OK|MB_ICONSTOP, NULL);
    return false;
  }
  DownCount = (unsigned) dato[0] + (unsigned) dato[1] * 256;
  DownCount++;
  if (!m_PES->WriteEEByte (28, DownCount%256)) {
    AfxMessageBox ("Chyba : Nelze zapsat do EEPROM", MB_OK|MB_ICONSTOP, NULL);
    return false;
  }
  if (!m_PES->WriteEEByte (29, DownCount/256)) {
    AfxMessageBox ("Chyba : Nelze zapsat do EEPROM", MB_OK|MB_ICONSTOP, NULL);
    return false;
  }

  /* Uložení názvu DNL souboru */
  ReportInfo ("SaveInfo ... nazev souboru\n");
  string dnl = m_strFilename;
  i = m_strFilename.rfind (PathFileSeparator);
  if (i >= 0) {
    dnl = "";
    i++;
    for (; i < (int) m_strFilename.size(); i++)
      dnl += m_strFilename[i];
  }
  dnl.erase (dnl.rfind ('.'), 4);

  for (i = 0; i < 8; i++)
    dato[i] = 0;
  for (i = 0; i < (int) dnl.size(); i++)
    dato[i] = dnl[i];

  for (i = 0; i < 8; i++) {
    if (!m_PES->WriteEEByte (30 + i, dato[i])) {
      AfxMessageBox ("Chyba : Nelze zapsat do EEPROM", MB_OK|MB_ICONSTOP, NULL);
      return false;
    }
  }

  /* Uložení data downloadu */
  ReportInfo ("SaveInfo ... datum downloadu\n");
  /*
  CTime t = CTime::GetCurrentTime();

  dato[0] = t.GetDay();
  dato[1] = t.GetMonth();
  dato[2] = t.GetYear() / 256;
  dato[3] = t.GetYear() % 256;
  dato[4] = t.GetHour();
  dato[5] = t.GetMinute();
  */
  time_t mtime;
  mtime = time (&mtime);
  struct tm * t = localtime (&mtime);
  dato[0] = t->tm_mday;
  dato[1] = t->tm_mon + 1;
  int year= t->tm_year + 1900;
  dato[2] = year / 256;
  dato[3] = year % 256;
  dato[4] = t->tm_hour;
  dato[5] = t->tm_min;
  for (i = 0; i < 6; i++) {
    if (!m_PES->WriteEEByte (38 + i, dato[i])) {
      AfxMessageBox ("Chyba : Nelze zapsat do EEPROM", MB_OK|MB_ICONSTOP, NULL);
      return false;
    }
  }
  return true;
}

bool CLoaderDlg::Lock (bool l) {
  ReportInfo ("Lock ... begin\n");

  if (!l) {
    BYTE  sekvence[18] = {
      0x55, 0x55, 0xaa,
      0x2a, 0xaa, 0x55,
      0x55, 0x55, 0x80,
      0x55, 0x55, 0xaa,
      0x2a, 0xaa, 0x55,
      0x55, 0x55, 0x20
    };
    if (!m_bNoLock) {
      if (!m_PES->WriteXBlock (sekvence, 0x0b00, 18)) {
        AfxMessageBox ("Chyba : Nelze odemknout programovou paměť!", MB_OK|MB_ICONSTOP, NULL);
        return false;

      }

      if (!m_PES->WriteEEByte (0xfc, 0)) {
        AfxMessageBox ("Chyba : Nelze odemknout programovou paměť!", MB_OK|MB_ICONSTOP, NULL);
        return false;
      }
    }
    if (!m_PES->WriteEEByte (27, 0x55)) {
      AfxMessageBox ("Chyba : Nelze odemknout programovou paměť!", MB_OK|MB_ICONSTOP, NULL);
      return false;
    }
  } else {
    BYTE  sekvence[9] = {
      0x55, 0x55, 0xaa,
      0x2a, 0xaa, 0x55,
      0x55, 0x55, 0xa0,
    };


    if (!m_PES->WriteEEByte (27, 0xaa)) {
      AfxMessageBox ("Chyba : Nelze uzamknout programovou paměť!", MB_OK|MB_ICONSTOP, NULL);
      return false;
    }

    if (!m_bNoLock) {
      if (!m_PES->WriteXBlock (sekvence, 0x0b00, 9)) {
        AfxMessageBox ("Chyba : Nelze uzamknout programovou paměť!", MB_OK|MB_ICONSTOP, NULL);
        return false;

      }
      if (!m_PES->WriteEEByte (0xfb, 0)) {
        AfxMessageBox ("Chyba : Nelze uzamknout programovou paměť!", MB_OK|MB_ICONSTOP, NULL);
        return false;
      }
    }
  }
  ReportInfo ("Lock ... OK\n");
  return true;
}




/*
void CLoaderDlg::OnStop() {
  m_bSTOP = true;
}
*/
bool CLoaderDlg::ReadPassword (string & psw) {
  BYTE  sum = 0;
  BYTE  dato[10];

  for (int i = 0; i < 8; i++) {
    if (!m_PES->ReadEEByte (EE_PasswordBegin + i, dato[i])) {
      AfxMessageBox ("Chyba : Nelze číst z EEPROM!", MB_OK|MB_ICONSTOP, NULL);
      return false;
    }
    sum += dato[i];
  }

  if (!m_PES->ReadEEByte (EE_PasswordCheck, dato[9])) {
    AfxMessageBox ("Chyba : Nelze číst z EEPROM!", MB_OK|MB_ICONSTOP, NULL);
    return false;
  }

  if (sum/0xFF == dato[9]) {
    // je to korektni heslo
    dato[9] = 0;
    psw = (char*) dato;
    ///psw.TrimRight();
    return true;
  }

  return false;
}

// vraci true, pokud je v automatu nastaven priznak
// ochrany heslem
bool CLoaderDlg::IsPassword() {
  BYTE  dato[2];

  if (m_PES->ReadEEByte (EE_PasswordEntredLo, dato[0])) {
    if (m_PES->ReadEEByte (EE_PasswordEntredHi, dato[1])) {
      if ( (dato[0] + dato[1]*256) == PASSWORD_ENTRED)
        return true;
      else
        return false;
    }
  }
  AfxMessageBox ("Chyba : Nelze číst z EEPROM!", MB_OK|MB_ICONSTOP, NULL);
  return true;
}
/*
bool CLoaderDlg::QuitAndPump() {
  MSG msg;
  while (::PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE)) {
    if (!theApp.PumpMessage()) {
      ::PostQuitMessage (0);
      return false;
    }
  }

  LONG  lIdle = 0;
  while (theApp.OnIdle (lIdle++));

  return true;

}
*/
