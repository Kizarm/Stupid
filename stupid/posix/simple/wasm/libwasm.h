#ifndef MY_EMLIB_H
#define MY_EMLIB_H
#include <stdarg.h>
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
// "imports" odpovídá importObject.imports v JS, default je to importObject.env
#define IMPORT(name) __attribute__((import_module("imports"),import_name(#name))) name
#define EXPORT(name) __attribute__((used, export_name(#name))) name
extern void * malloc (unsigned long n);
extern void   free   (void * p);
extern char * _HEAP_MAX;
/** Pozor - vše je _velmi_ zjednodušeno. Zase je to docela malé.
 * fmt v printf a vsnprintf umí jen (unsigned) int formáty
 * */
  extern void * calloc (unsigned long nmemb, unsigned long size);
  extern void * realloc(void *ptr, unsigned long size);

  typedef __SIZE_TYPE__ size_t;
  #define NULL   ((void *)0)
  size_t strlen (const char *s);
  extern char * strncpy (char *dst0, const char *src0, size_t count);
  extern int    strncmp (const char *s1, const char *s2, size_t n);
  extern int    strcmp  (const char *s1, const char *s2);
  extern void * memcpy  (void *dest, const void *src, size_t n);
  extern void * memset  (void *s, int c, size_t n);
  extern char * strtok  (char *s, const char *delim);
  
  extern signed int vsnprintf (char *pStr, size_t length, const char *fmt, va_list ap);
  extern signed int  snprintf (char *pStr, size_t length, const char *fmt, ...);
  extern int puts   (const char * ptr);
  extern int printf (const char *__restrict fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
  
#ifdef __cplusplus
};
#endif //__cplusplus
#endif // MY_EMLIB_H
