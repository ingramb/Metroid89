// C Header File
// Created 10/2/01; 8:47:35 PM

#ifndef __UTILITY__
#define __UTILITY__

typedef struct {
	unsigned short player_action_offset;
	unsigned short enemy_action_offset;
	unsigned short enemy_data_offset;
	unsigned short shot_data_offset;
	unsigned short combo_data_offset;
	unsigned short bg_list_offset;
} METPACK_HEADER;

#include "dll.h"
#include "dllexports.h"

#define set_game_speed(speed) poke(0x600017,speed)
#define absolute(a) ((a)>0 ? (a) : -(a))
#define unlock_file(file) HeapUnlock(SymFindPtr(SYMSTR(file),0)->handle)

//==================================================
//Constants for low-level keyboard access
//==================================================
#define ARROWS_ROW 0xfffe
#define LEFT_KEY 2
#define RIGHT_KEY 8
#define UP_KEY 1
#define DOWN_KEY 4
#define SEL_KEY 16
#define SHIFT_KEY 32
#define DMND_KEY 64
#define ESC_ROW 0xffbf
#define ESC_KEY 1
#define APPS_ROW 0xffdf
#define APPS_KEY 1
#define BSPACE_ROW 0xfffb
#define CLEAR_ROW 0xfffd
#define BSPACE_KEY 64
#define CLEAR_KEY 64


#define NONE -1

#define LEFT 1
#define RIGHT 2
#define UP 4
#define DOWN 8

#ifndef __IN_DLL__

char gfx_setup();
void gfx_cleanup();
void flipping_off();
void flipping_on();
void clear_buffer();
void draw_number(short x, short y, unsigned short number, char *font, char digits, void *light, void *dark);
void update_screen();
void pause();
void delay(short a);
char file_exists(const char *file);
void *file_pointer(const char *file);
void error(char *txt);

void dll_cleanup();
char load_metgame();
char load_metsys();
char load_metpack();

short __attribute__((__stkparm__)) OldUnpackBuffer(unsigned char *src, unsigned char *dest);

#define decompress OldUnpackBuffer

#endif

//#undef DEFINE_INT_HANDLER
//#define DEFINE_INT_HANDLER(name) extern _DEREF_INT_HANDLER name[]; asm(".xdef __ref_all___custom_int_handlers;.text;.even;"#name":move.w #0x2700,%sr;movem.l %d0-%d7/%a0-%a6,-(%sp);move.l 0xC8,%a0;jbsr __"#name"_body__;movem.l (%sp)+,%d0-%d7/%a0-%a6;rte"); void __##name##_body__(void)

#endif


