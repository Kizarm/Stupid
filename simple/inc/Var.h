#if !defined(AFX_VAR_H__INCLUDED_)
#define AFX_VAR_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum iden{
           I_type,   I_ntyp,   I_cons,   I_var,    I_mac
         };

enum typ{
           T_subr,   T_enum,   T_ord,    T_real,   T_str,
           T_arr,    T_rec,    T_set,    T_ptr,    T_proc,
           T_fce,    T_mac
         };

enum clas{ 
           C_undef,  C_ext,    C_stat,   C_auto
         };

#define    L_int    (16)
#define    L_word   (16)
#define    L_bool   (1)
#define    L_real   (32)
#define    L_ptr    (16)
#define    L_bptr   (32)
#define    L_char   (8)

#define    L_enum   (16L)

enum typnum
         {
           N_int,    N_word,   N_wori,   N_bool,   N_boli,   N_real,
           N_last
         };

#endif // AFX_VAR_H__INCLUDED_