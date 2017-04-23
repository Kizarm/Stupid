/*###########################################################################*/
/*###########################################################################*/
/*                                                                           */
/*   vazba na firmware 30.04                                                 */
/*                                                                           */
/*###########################################################################*/
/*###########################################################################*/
#if !defined(AFX_FIRMWARE_H__INCLUDED_)
#define AFX_FIRMWARE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define RomTab  (0x400)
#define ROMORG  (0x8400)
#define LAC     (0x20)
#define HSP     (0x07)

enum FFunkce {

           WDISP    ,
           SDISP    ,
           WMULTR   ,
           WDIVR    ,
           LOADNET  ,

           SYSTEM   ,
           TPOLE    ,

           XRPUSH   ,
           XAPUSH   ,
           XBPUSH   ,

           XRPOP    ,
           XAPOP    ,
           XBPOP    ,

           FSTART   ,
           FPRETW   ,
           FPRETR   ,
           FRETW    ,
           FRETR    ,
           PRET     ,

           LDBAA    ,
           LDAA     ,

           LDRD     ,
           LDRI     ,
           STRD     ,
           STRI     ,

           LDBI     ,
           STBI     ,
           RESBI    ,
           SETBI    ,
           INVBI    ,

           WLTR     ,
           WLT      ,
           WGTR     ,
           WGT      ,
           WNE      ,
           ILTR     ,
           ILT      ,
           IGTR     ,
           IGT      ,

           WADD     ,
           WSUB     ,
           WMULT    ,
           WDIV     ,
           IDIVR    ,
           IDIV     ,

           RADD     ,
           RSUB     ,
           RMULT    ,
           RDIV     ,

           RCHS     ,
           RABS     ,
           RTRUN    ,

           REQ      ,
           RNE      ,
           RLT      ,
           RGT      ,

           ITOR     ,
           WTOR     ,
           RTOI     ,
           RTOW     ,

           SSIN     ,
           SCOS     ,
           SLOG     ,
           SEXP     ,
           SSQRT    ,

           XDISP    , /* 07.05.98 */
           XPRNT    , /* 08.04.98 */
           SPRNT    ,
		   
		   /* 20.02.04 */
		   WAND     , 
		   WANDR    ,
		   WOR      ,
		   WORR     ,
		   WXOR     ,
		   WXORR    ,

           MAXFCE
           };

static const char * const RomFName[MAXFCE] = {

          "WDISP  ",
          "SDISP  ",
          "WMULTR ",
          "WDIVR  ",
          "LOADNET",

          "SYSTEM ",
          "TPOLE  ",

          "XRPUSH ",
          "XAPUSH ",
          "XBPUSH ",

          "XRPOP  ",
          "XAPOP  ",
          "XBPOP  ",

          "FSTART ",
          "FPRETW ",
          "FPRETR ",
          "FRETW  ",
          "FRETR  ",
          "PRET   ",

          "LDBAA  ",
          "LDAA   ",

          "LDRD   ",
          "LDRI   ",
          "STRD   ",
          "STRI   ",

          "LDBI   ",
          "STBI   ",
          "RESBI  ",
          "SETBI  ",
          "INVBI  ",

          "WLTR   ",
          "WLT    ",
          "WGTR   ",
          "WGT    ",
          "WNE    ",
          "ILTR   ",
          "ILT    ",
          "IGTR   ",
          "IGT    ",

          "WADD   ",
          "WSUB   ",
          "WMULT  ",
          "WDIV   ",
          "IDIVR  ",
          "IDIV   ",

          "RADD   ",
          "RSUB   ",
          "RMULT  ",
          "RDIV   ",

          "RCHS   ",
          "RABS   ",
          "RTRUN  ",

          "REQ    ",
          "RNE    ",
          "RLT    ",
          "RGT    ",

          "ITOR   ",
          "WTOR   ",
          "RTOI   ",
          "RTOW   ",

          "SSIN   ",
          "SCOS   ",
          "SLOG   ",
          "SEXP   ",
          "SSQRT  ",

          "XDISP  ",
          "XPRNT  ",
          "SPRNT  " 
		  
		  "WAND   ", 
		  "WANDR  ",
		  "WOR    ",
		  "WORR   ",
		  "WXOR   ",
		  "WXORR  "

          };
#endif // AFX_FIRMWARE_H__INCLUDED_