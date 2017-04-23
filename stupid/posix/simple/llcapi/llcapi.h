#ifndef LLCAPI_H_
#define LLCAPI_H_
#include "../Compiler.h"

extern "C" int CompileLLtoASFile (const char * infile, const char* outfile, LLVMTypeMachine f);

#endif // LLCAPI_H_
