extern struct _Item {
	char Tag; Exp E; byte Map; word Line, File;
	Segment Seg; word Offset;
} *RTab;
typedef struct _Item *Item;

extern int RCur;

extern long LVal;

extern void Resolve(Item IP);
extern void Reloc(byte Code, byte Tag, Exp E);
extern void ResInit(void);

