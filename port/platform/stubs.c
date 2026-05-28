// Phase-0 placeholder implementations of the TIGCC/ExtGraph shim surface.
// These let the full game link and start up; each is replaced by a real
// implementation in later phases (tifile.c, gfx_shim.c, sprite_shim.c, ...).

#include "tigcc_compat.h"
#include "globals.h"   // game types (TILE_DATA, TILE_NODE, ...) for draw_map1

// ---------------------------------------------------------------------------
// LCD video memory backing buffer
// ---------------------------------------------------------------------------
static unsigned char lcd_buffer[LCD_SIZE];
unsigned char *LCD_MEM = lcd_buffer;

// ---------------------------------------------------------------------------
// Grayscale double buffer: two hidden planes + two active planes.
// ---------------------------------------------------------------------------
static unsigned char gray_hidden[2][PLANE_SIZE];
static unsigned char gray_active[2][PLANE_SIZE];

void  GrayOn(void) {}
void  GrayOff(void) {}
void  GrayAdjust(short v) { (void)v; }
void  GrayDBufInit(void *buf) { (void)buf; }
void *GrayDBufGetHiddenPlane(short p) { return gray_hidden[p & 1]; }
void *GrayDBufGetActivePlane(short p) { return gray_active[p & 1]; }
void  GrayDBufToggleSync(void) {}
void  PortSet(void *p, short a, short b) { (void)p; (void)a; (void)b; }
void  PortRestore(void) {}

// Heap / symbol table / TTArchive / decompress: implemented in tifile.c and
// ttunpack_port.c.

// ---------------------------------------------------------------------------
// Console / RNG  (_rowread lives in main_sdl.c with the SDL keyboard state)
// ---------------------------------------------------------------------------
int   ngetchx(void) { return 0; }
void  clrscr(void) {}
void  ClrScr(void) {}
void  printf_xy(short x, short y, const char *fmt, ...) { (void)x; (void)y; (void)fmt; }
void  ST_helpMsg(const char *msg) { if (msg) fprintf(stderr, "%s\n", msg); }
#undef random
short port_random(short n) { return n ? (short)(rand() % n) : 0; }
void  randomize(void) { srand(1); }

// ---------------------------------------------------------------------------
// Interrupt vector bookkeeping (timing handled by SDL timer later)
// ---------------------------------------------------------------------------
INT_HANDLER GetIntVec(short n) { (void)n; return DUMMY_HANDLER; }
void        SetIntVec(short n, INT_HANDLER h) { (void)n; (void)h; }
void        ExecuteHandler(INT_HANDLER h) { if (h) h(); }

// FastDraw* / GraySprite8 / GraySprite16 / SpriteX8_MIRROR_H: see sprite_shim.c.

// The three clip-MASK routines that were hand-written m68k asm (missing .s files).
void GraySpriteClip8_MASK (short x,short y,short h,unsigned char*s,void*lb,void*db){(void)x;(void)y;(void)h;(void)s;(void)lb;(void)db;}
void GraySpriteClip16_MASK(short x,short y,short h,unsigned short*s,void*lb,void*db){(void)x;(void)y;(void)h;(void)s;(void)lb;(void)db;}
void GraySpriteClip32_MASK(short x,short y,short h,unsigned long*s,void*lb,void*db){(void)x;(void)y;(void)h;(void)s;(void)lb;(void)db;}

// draw_map1 is now implemented in DLLMap.c.

