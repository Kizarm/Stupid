#include "libwasm.h"
extern void IMPORT(memoryGrow) (const int block);
extern char __heap_base;

static const char * _HEAP_START = &__heap_base;
char *              _HEAP_MAX   = &__heap_base;
#define NULL   ((void *)0)
typedef unsigned long ALIGN;

union header {
  struct {
    union header  * ptr;
    unsigned long   size;
  } s;
  ALIGN             x;
};

typedef union header HEADER;

static HEADER   base;
static HEADER * allocp = NULL;

#define NALLOC 64

static HEADER * morecore (unsigned long);
/* K&R allocator */
void * malloc (unsigned long nbytes) {
  HEADER *p, *q;                // K&R called q, prevp
  unsigned nunits;
  
  nunits = (nbytes + sizeof (HEADER) - 1) / sizeof (HEADER) + 1;
  
  if ((q = allocp) == NULL) {   // no free list yet 
    base.s.ptr = allocp = q = &base;
    base.s.size = 0;
  }
  for (p = q->s.ptr;; q = p, p = p->s.ptr) {
    if (p->s.size >= nunits) {  // big enough 
      if (p->s.size == nunits)  // exactly 
        q->s.ptr = p->s.ptr;
      else {                    // allocate tail end
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size  = nunits;
      }
      allocp = q;
      return ((char *) (p + 1));
    }
    if (p == allocp) {
      if ((p = morecore (nunits)) == NULL)  return NULL;
    }
  }
}
void free (void *ap) {
  HEADER *p, *q;
  p = (HEADER *) ap - 1;
  for (q = allocp; ! (p > q && p < q->s.ptr); q = q->s.ptr)
    if (q >= q->s.ptr && (p > q || p < q->s.ptr))
      break;

  if (p + p->s.size == q->s.ptr) {
    p->s.size += q->s.ptr->s.size;
    p->s.ptr = q->s.ptr->s.ptr;
  } else
    p->s.ptr = q->s.ptr;
  if (q + q->s.size == p) {
    q->s.size += p->s.size;
    q->s.ptr = p->s.ptr;
  } else
    q->s.ptr = p;
  allocp = q;
}

static void * sbrk (unsigned long size) {
  static const char * heap_ptr;
  const char *        old_heap_ptr;
  static unsigned int init_sbrk = 0;
  /* heap_ptr is initialized to HEAP_START */
  if (init_sbrk == 0) {
    heap_ptr = _HEAP_START;
    init_sbrk = 1;
  }
  old_heap_ptr = heap_ptr;
  /* Tohle je jen zkusmo, uvidíme, zatím se zdá, že to chodí. 
   * Těžko říct, co to udělá, když dojde paměť, ale pár MiB to zvládne.
   */
  if ((heap_ptr + size) > _HEAP_MAX) {
    const int blocks = (((heap_ptr + size) - _HEAP_MAX) >> 16) + 1;
    memoryGrow (blocks);
    _HEAP_MAX += blocks << 16;
  }  
  heap_ptr += size;
  return (void *)old_heap_ptr;
}

static HEADER * morecore (unsigned long nu) {
  char   * cp;
  HEADER * up;
  int      rnu;

  rnu = NALLOC * ((nu + NALLOC - 1) / NALLOC);
  //printf("morecore %ld units, %d rnu\n", nu, rnu);
  cp  = sbrk (rnu * sizeof (HEADER));
  if (cp == NULL) return NULL;
  up = (HEADER *) cp;
  up->s.size = rnu;
  free ((char *) (up + 1));
  return (allocp);
}
void* calloc (unsigned long nmemb, unsigned long size) {
  unsigned long i, req = nmemb * size;
  if (!req) req++;
  void* res = malloc(req);
  char* clr = (char*) res;
  for (i=0; i<req; i++) clr[i] = 0;
  return res;
}
/// pouze zkraceni, pri prodlozeni to muze cist nekde kde nema
void *realloc(void *ptr, unsigned long size) {
  unsigned char* nptr = malloc (size);
  unsigned char* optr = (unsigned char*) ptr;
  int i;
  for (i=0; i<size; i++) nptr[i] = optr[i];
  free (ptr);
  return (void*) nptr;
}

