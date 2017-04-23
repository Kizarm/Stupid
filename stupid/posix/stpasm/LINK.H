struct _FileBuf {
   char *Name;
   long Loc, Files, Segs, Gaps, Syms, Exps, Relocs;
   char **File; Segment Seg; Gap G; Symbol *Sym;
};
typedef struct _FileBuf *FileBuf;
extern FileBuf FTab;
extern int Fs;
extern void Link(char *Hex);
