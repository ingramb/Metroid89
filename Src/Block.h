// C Header File
// Created 10/21/2002; 7:25:07 PM

#ifndef __BLOCK__
#define __BLOCK__

#include "dllexports.h"

typedef struct {
	short x;
	short y;
	short width;
	short hieght;
	short next;
} BLOCK;

#define BLOCK_MAX 15
#define BLOCK_STRETCH 1
#define BLOCK_SQUISH (-1)

#ifndef __IN_DLL__
#define block_setup _DLL_call(char, (), BLOCK_SETUP)
#define block_reset _DLL_call(void, (), BLOCK_RESET)
#define block_cleanup _DLL_call(void, (), BLOCK_CLEANUP)
#define block_create _DLL_call(short, (short, short, short, short), BLOCK_CREATE)
#define block_stretch_vert _DLL_call(void, (short, short, short), BLOCK_STRETCH_VERT)
#define block_stretch_horiz _DLL_call(void, (short, short, short), BLOCK_STRETCH_HORIZ)
#define block_move _DLL_call(void, (short, short, short), BLOCK_MOVE)
#define block_delete _DLL_call(void, (short), BLOCK_DELETE)
#define block_test _DLL_call(char, (short, short, short, short, short, short), BLOCK_TEST)
#define block_check _DLL_call(char, (short, short, short, short, short), BLOCK_CHECK)
#endif

#endif