#include <stdio.h>
#include <string.h>
#include "AsmApi.h"

#include "IO.H"
#include "OP.H"
#include "ST.H"
#include "LINK.H"


static void IDEFilter (void) {
  errout = fopen ("ERRLIST.TXT","w");
}


static void PostFunc (void) {
  fclose (errout);
}


int AsmHexApi (const char * filename) {
  int A = 0;
  char *Hex;
  byte DoHex = 0, DoLink = 1;
  Fs = 1;
  
  IDEFilter();
  
  char name [1024];
  strncpy (name, filename, 1024);
  
  FTab = (FileBuf) Allocate (Fs * sizeof *FTab);
  OpInit();
  for (A = 0; A < Fs; A++) {
    char * S, *Src, *Obj, Ch='\0';

    Src = name;
    for (S = Src + strlen (Src) - 1; S > Src; S--)
      if (*S == '.') break;
    if (strcmp (S, ".o") == 0) Obj = CopyS (Src), Src = 0;
    else {
      if (S > Src) Ch = *S, *S = '\0';
      Obj = (char *) Allocate (strlen (Src) + 3);
      sprintf (Obj, "%s.o", Src);
      if (S > Src) *S = Ch;
    }
    if (DoLink) FTab[A].Name = Obj;
    if (Src != 0) {
      OutF = OpenObj (Obj);
      if (OutF == 0) {
        fprintf (errout, "Cannot open object file for %s.\n", Src);
        fflush (stdout);
        fflush (stderr);
        return -1;
      }
      Assemble (Src), Generate();
    }
  }
  CHECK();
  if (DoLink) {
    if (!DoHex) {
      char * S, *Obj = FTab[0].Name, Ch;
      //- word L = strlen (Obj) + 3;
      for (S = Obj + strlen (Obj) - 1; S > Obj; S--)
        if (*S == '.') break;
      if (S > Obj) Ch = *S, *S = '\0';
      Hex = (char *) Allocate (strlen (Obj) + 5);
      sprintf (Hex, "%s.hex", Obj);
      if (S > Obj) *S = Ch;
    }
    fprintf (errout, "linking");
    fflush (stdout);
    fflush (stderr);
    for (A = 0; A < Fs; A++) {
      fputc (' ', errout);
      fprintf (errout , "%s", FTab[A].Name);
      fflush (stdout);
      fflush (stderr);
    }
    Link (Hex);
  }
  
  PostFunc ();
  return 0;
}
/*
int main (void) {
  // test
  return AsmHexApi ("Program.mac");
}
*/