// Minimal Allegro stub so the MapEditor's data modules compile headless.
// Only the map-structure export path is exercised; all the drawing/keyboard
// entry points are present just to satisfy the compiler/linker and do nothing.

#ifndef MAPCONV_ALLEGRO_STUB_H
#define MAPCONV_ALLEGRO_STUB_H

#include <stdlib.h>
#include <stdarg.h>

typedef struct BITMAP { int w, h; } BITMAP;
typedef struct RLE_SPRITE { int w, h; } RLE_SPRITE;
typedef struct RGB { unsigned char r, g, b, filler; } RGB;
typedef RGB PALETTE[256];
typedef struct FONT { int dummy; } FONT;

extern BITMAP *screen;
extern FONT   *font;
extern char    key[256];

#define KEY_SPACE 1
#define KEY_ESC   2
#define KEY_ENTER 3

static inline int  makecol(int r, int g, int b) { (void)r;(void)g;(void)b; return 0; }
static inline void clear(BITMAP *b) { (void)b; }
static inline void clear_to_color(BITMAP *b, int c) { (void)b;(void)c; }
static inline void textprintf(BITMAP *b, FONT *f, int x, int y, int c, const char *fmt, ...) { (void)b;(void)f;(void)x;(void)y;(void)c;(void)fmt; }
static inline void textout(BITMAP *b, FONT *f, const char *s, int x, int y, int c) { (void)b;(void)f;(void)s;(void)x;(void)y;(void)c; }
static inline void putpixel(BITMAP *b, int x, int y, int c) { (void)b;(void)x;(void)y;(void)c; }
static inline int  getpixel(BITMAP *b, int x, int y) { (void)b;(void)x;(void)y; return 0; }
static inline void line(BITMAP *b, int x1, int y1, int x2, int y2, int c) { (void)b;(void)x1;(void)y1;(void)x2;(void)y2;(void)c; }
static inline void rect(BITMAP *b, int x1, int y1, int x2, int y2, int c) { (void)b;(void)x1;(void)y1;(void)x2;(void)y2;(void)c; }
static inline void rectfill(BITMAP *b, int x1, int y1, int x2, int y2, int c) { (void)b;(void)x1;(void)y1;(void)x2;(void)y2;(void)c; }
static inline void blit(BITMAP *s, BITMAP *d, int sx, int sy, int dx, int dy, int w, int h) { (void)s;(void)d;(void)sx;(void)sy;(void)dx;(void)dy;(void)w;(void)h; }
static inline BITMAP *load_bitmap(const char *f, RGB *pal) { (void)f;(void)pal; return 0; }
static inline BITMAP *create_bitmap(int w, int h) { (void)w;(void)h; return 0; }
static inline void destroy_bitmap(BITMAP *b) { (void)b; }
static inline RLE_SPRITE *get_rle_sprite(BITMAP *b) { (void)b; return 0; }
static inline void destroy_rle_sprite(RLE_SPRITE *s) { (void)s; }
static inline void draw_rle_sprite(BITMAP *b, RLE_SPRITE *s, int x, int y) { (void)b;(void)s;(void)x;(void)y; }

#endif

// --- additions: defines + Allegro GUI/draw symbols (unused headlessly) ---
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef struct DIALOG {
    int (*proc)(int, struct DIALOG *, int);
    int x, y, w, h, fg, bg, key, flags, d1, d2;
    char *dp; char *dp2; char *dp3;
} DIALOG;

#define D_EXIT      1
#define D_O_K       0
#define D_REDRAW    2
#define D_GOTFOCUS  0
#define D_SELECTED  1

static inline int d_box_proc(int m, DIALOG *d, int c){ (void)m;(void)d;(void)c; return 0; }
static inline int d_text_proc(int m, DIALOG *d, int c){ (void)m;(void)d;(void)c; return 0; }
static inline int d_edit_proc(int m, DIALOG *d, int c){ (void)m;(void)d;(void)c; return 0; }
static inline int d_check_proc(int m, DIALOG *d, int c){ (void)m;(void)d;(void)c; return 0; }
static inline int d_radio_proc(int m, DIALOG *d, int c){ (void)m;(void)d;(void)c; return 0; }
static inline int d_button_proc(int m, DIALOG *d, int c){ (void)m;(void)d;(void)c; return 0; }
static inline int do_dialog(DIALOG *d, int focus){ (void)d;(void)focus; return 0; }
static inline void centre_dialog(DIALOG *d){ (void)d; }
static inline void vline(BITMAP *b,int x,int y1,int y2,int c){ (void)b;(void)x;(void)y1;(void)y2;(void)c; }
static inline void hline(BITMAP *b,int x1,int y,int x2,int c){ (void)b;(void)x1;(void)y;(void)x2;(void)c; }
static inline void draw_lit_rle_sprite(BITMAP *b,RLE_SPRITE *s,int x,int y,int color){ (void)b;(void)s;(void)x;(void)y;(void)color; }

#define DRAW_MODE_SOLID 0
#define DRAW_MODE_TRANS 1
extern int mouse_b;
static inline void drawing_mode(int mode, BITMAP *pat, int x, int y){ (void)mode;(void)pat;(void)x;(void)y; }
static inline void stretch_sprite(BITMAP *d, BITMAP *s, int x, int y, int w, int h){ (void)d;(void)s;(void)x;(void)y;(void)w;(void)h; }
static inline void draw_trans_rle_sprite(BITMAP *b, RLE_SPRITE *s, int x, int y){ (void)b;(void)s;(void)x;(void)y; }
static inline char *_strtime(char *s){ if(s) s[0]=0; return s; }
