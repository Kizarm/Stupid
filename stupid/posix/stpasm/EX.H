typedef enum { NumX, AddrX, SymX, UnX, BinX, CondX } ExpTag;
struct _Exp {
   word Hash, Line, File; int Mark:1, Map:1;
   ExpTag Tag; struct _Exp * Tail, * Next;
   union {
      word Value;
      struct { Segment Seg; word Offset; } Addr;
      Symbol Sym;
      struct { Lexical Op; struct _Exp * A, * B, * C; } Funct;
   } Body;
};
typedef struct _Exp *Exp;
extern Exp ExpHead;

#define VALUE(E)  ((E)->Body.Value)
#define SEG(E)    ((E)->Body.Addr.Seg)
#define OFFSET(E) ((E)->Body.Addr.Offset)
#define SYM(E)    ((E)->Body.Sym)
#define OP(E)     ((E)->Body.Funct.Op)
#define ARG1(E)   ((E)->Body.Funct.A)
#define ARG2(E)   ((E)->Body.Funct.B)
#define ARG3(E)   ((E)->Body.Funct.C)

extern Exp Parse(int Dir);
extern Exp MakeExp(ExpTag Tag, ...);
extern Exp EvalExp(Exp E);
extern void MarkExp(Exp E);
extern void ExpInit(void);
