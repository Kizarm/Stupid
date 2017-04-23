/*###########################################################################*/
/*    funkce prace se seznamem                                               */
/*###########################################################################*/


#if !defined(AFX_LIST_H__INCLUDED_)
#define AFX_LIST_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define  AR_DEL   (200)

typedef  long int ATRIBUT;

typedef struct pol{
       struct pol *prev     ; /* Ukazatel na predchozi polozku seznamu       */
       char       *iden     ; /* Ukazatel na string identifikatoru           */
       int         typ      ; /* Typ identifikatoru                          */
       int         podtyp   ; /* Podtyp identifikatoru                       */
       char       *dtyp     ; /* Ukazatel na popis typu                      */
       long        size     ; /* Velikost objektu                            */
       int         hloubka  ; /* Hloubka ve ktere je id definovan            */
       int         trida    ; /* Trida pameti                                */
       ATRIBUT     atr      ; /* Adresa, nebo offset                         */
       float       fl       ; /* pro float cisla                             */
       int         fpl      ; /* pocet bitu formalnich (skut) parametru      */
       int         rvs      ; /* pocet bitu vracene hodnoty funkci           */
       int         hz       ; /* hloubka zanoreni procesury ci funkce        */
       int         hst      ; /* potrebna hloubka zasobniku                  */
       } POLOZKA ;

typedef struct lpol{
       char        iden[AR_DEL] ; /* Ukazatel na string identifikatoru       */
       int         typ          ; /* Typ identifikatoru                      */
       int         podtyp       ; /* Podtyp identifikatoru                   */
       char        dtyp[AR_DEL] ; /* Ukazatel na popis typu                  */
       long        size         ; /* Velikost objektu                        */
       int         hloubka      ; /* Hloubka ve ktere je id definovan        */
       int         trida        ; /* Trida pameti                            */
       ATRIBUT     atr          ; /* Atribut (Adresa, offset atd. )          */
       float       fl       ; /* pro float cisla                             */
       int         fpl      ; /* pocet bitu formalnich (skut) parametru      */
       int         rvs      ; /* pocet bitu vracene hodnoty funkci           */
       int         hz       ; /* hloubka zanoreni procesury ci funkce        */
       int         hst      ; /* potrebna hloubka zasobniku                  */
       } LPOLOZKA ;

typedef struct slpol{
       char        iden[12]     ; /* Ukazatel na string identifikatoru       */
       int         typ          ; /* Typ identifikatoru                      */
       int         podtyp       ; /* Podtyp identifikatoru                   */
       char        dtyp[12]     ; /* Ukazatel na popis typu                  */
       long        size         ; /* Velikost objektu                        */
       int         hloubka      ; /* Hloubka ve ktere je id definovan        */
       int         trida        ; /* Trida pameti                            */
       ATRIBUT     atr          ; /* Atribut (Adresa, offset atd. )          */
       float       fl       ; /* pro float cisla                             */
       int         fpl      ; /* pocet bitu formalnich (skut) parametru      */
       int         rvs      ; /* pocet bitu vracene hodnoty funkci           */
       int         hz       ; /* hloubka zanoreni procesury ci funkce        */
       int         hst      ; /* potrebna hloubka zasobniku                  */
       } SPOLOZKA ;

/*---------------------------------------------------------------------------*/
/*    Exportovane moduly                                                     */
/*---------------------------------------------------------------------------*/

extern POLOZKA  *sez_uk;    // ukazatel na naposled manipulovanou polozku
extern POLOZKA  *SezDummy;  // ukazatel na pomocnou polozku
extern POLOZKA  *Last  ;    // ukazatel na posledni polozku seznamu
extern int      Hloubka ;   // obsahuje aktualni hloubku zanoreni modulu


extern void     SezNalezen(char *Identifikator);
extern void     SezTypu(int TypPolozky);
extern void     SezPodtypu(int PodtypPolozky);
extern void     SezUloz(LPOLOZKA *p);
extern void		SezSUloz(SPOLOZKA *p);
extern POLOZKA *SezUlozId(LPOLOZKA *p);
extern void     SezZapomenHladinu( int HloubkaHladiny );
extern void     SezZapomenVse();
extern void     SezInit();
extern void		SezDopln(POLOZKA *n,LPOLOZKA *p);

#endif// AFX_LIST_H__INCLUDED_