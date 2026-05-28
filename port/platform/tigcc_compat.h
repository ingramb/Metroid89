// Compatibility shim that replaces TIGCC's <tigcclib.h>/<extgraph.h>/<intr.h>/<dll.h>
// for the native SDL port. Provides the TI/AMS/ExtGraph types, macros, and function
// declarations the game source expects. Implementations live in port/platform/*.c.

#ifndef PORT_TIGCC_COMPAT_H
#define PORT_TIGCC_COMPAT_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "screen.h"

// ---------------------------------------------------------------------------
// Basic constants the calculator headers assumed from tigcclib
// ---------------------------------------------------------------------------
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

// LCD: the calculator video memory. We back it with a real buffer.
#define LCD_SIZE 3840
extern unsigned char *LCD_MEM;

// Hardware identification. Native build behaves like a hw1 TI-89.
#define HW_VERSION 1
#define CALCULATOR 0

// Size of the gray double-buffer block GrayDBufInit() manages.
#define GRAYDBUFFER_SIZE (PLANE_SIZE * 4 + 16)

// Drawing modes (extgraph / AMS).
enum {
    A_REPLACE = 0,
    A_NORMAL,
    A_REVERSE,
    A_XOR,
    A_OR,
    A_AND,
    A_SHADE,
    A_SHADED,
    A_THICK,
};

// 4-gray "colors" used by the line/box drawing helpers.
enum {
    COLOR_WHITE = 0,
    COLOR_LIGHTGRAY,
    COLOR_DARKGRAY,
    COLOR_BLACK,
};

// ---------------------------------------------------------------------------
// AMS heap / symbol table types
// ---------------------------------------------------------------------------
typedef unsigned long HANDLE;
#define H_NULL ((HANDLE)0)

typedef struct {
    HANDLE handle;
    unsigned short flags;
    char name[12];
} SYM_ENTRY;

typedef struct {
    unsigned short folder;
    unsigned short offset;
} HSym;

#define SYMSTR(s) (s)

// Variable type tag byte trailing AMS variables (value irrelevant natively).
#define OTH_TAG 0x2C

// Symbol table / heap (implemented in tifile.c over on-disk .89z files).
HSym       SymFind(const char *name);
SYM_ENTRY *SymFindPtr(const char *name, short flags);
HSym       SymAdd(const char *name);
SYM_ENTRY *DerefSym(HSym h);
void      *HLock(HANDLE h);
void       ti_unlock_file(const char *name);   // null-safe HeapUnlock by name
void      *HeapDeref(HANDLE h);
short      HeapLock(HANDLE h);
short      HeapUnlock(HANDLE h);
short      HeapGetLock(HANDLE h);
HANDLE     HeapPtrToHandle(void *p);
HANDLE     HeapAlloc(unsigned long size);
HANDLE     HeapAllocHigh(unsigned long size);
void       HeapFree(HANDLE h);
HANDLE     HeapRealloc(HANDLE h, unsigned long size);
unsigned long HeapMax(void);
unsigned long HeapAvail(void);
void       HeapCompress(void);

// ---------------------------------------------------------------------------
// Memory-mapped poke/peek -> no-ops / harmless on native
// ---------------------------------------------------------------------------
#define poke(addr, val)   ((void)0)
#define pokeIO(addr, val) ((void)0)
#define peek(addr)        (0)
#define peek_w(addr)      (0)

// ---------------------------------------------------------------------------
// Keyboard / system
// ---------------------------------------------------------------------------
short _rowread(short mask);
int   ngetchx(void);
void  clrscr(void);
void  ClrScr(void);
void  printf_xy(short x, short y, const char *fmt, ...);
void  ST_helpMsg(const char *msg);

// 'random' collides with BSD libc random(); route through our own symbol.
short port_random(short n);
#define random(n) port_random(n)
void  randomize(void);

// ---------------------------------------------------------------------------
// Grayscale double-buffer (GrayDBuf) emulation
// ---------------------------------------------------------------------------
void  GrayOn(void);
void  GrayOff(void);
void  GrayAdjust(short v);
void  GrayDBufInit(void *buf);
void *GrayDBufGetHiddenPlane(short plane);
void *GrayDBufGetActivePlane(short plane);
void  GrayDBufToggleSync(void);

// Grayscale hardware port save/restore (no-ops natively).
void PortSet(void *p, short a, short b);
void PortRestore(void);

#include "intr.h"
#include "extgraph.h"

#endif
