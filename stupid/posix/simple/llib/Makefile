simpledef.ll: simpledef.c
	clang-3.8 -S -O3 --target=i386-- -emit-llvm simpledef.c -o tmp.ll
	sed -r -f ll.sed tmp.ll >simpledef.ll
	rm tmp.ll

all: simpledef.ll
clean:
	rm -f *.ll

