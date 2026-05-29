// C Source File
// Created 11/19/2002; 1:08:02 PM

// C Source File
// Created 10/1/01; 9:01:10 PM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
#include <extgraph.h>
#include "miscgfx.h"
#include "player.h"
#include "utility.h"
#include "globals.h"

//void *dll_state = NULL;

void *dll_imports[] = {
	gfx_setup,
	gfx_cleanup,
	flipping_off,
	flipping_on,
	clear_buffer,
	update_screen,
	pause,
	delay,
	file_exists,
	file_pointer,
	error,
	
	player_move,

	SpriteX8,
	GraySpriteX8,
	GraySpriteX8_mask,
	decompress,
};

DEFINE_INT_HANDLER(speed_timer)
{
	glbs_base->game_counter++;
}

DEFINE_INT_HANDLER(fps_timer_hw1)
{
	//glbs = glbs_base;
	if(glbs_base->timer > 0) glbs_base->timer--;
	glbs_base->fps_counter++;
	if(glbs_base->fps_counter >= 350){
		glbs_base->fps_counter = 0;
		glbs_base->current_fps = glbs->frames;
		glbs_base->frames = 0;
		glbs_base->seconds++;
	}
}

DEFINE_INT_HANDLER(fps_timer_hw2)
{
	glbs = glbs_base;
	glbs->current_fps = glbs->frames;
	glbs->frames = 0;
	glbs->seconds++;
	ExecuteHandler(glbs->save_int[2]);
}

DEFINE_INT_HANDLER(timer_hw2)
{
	glbs = glbs_base;
	if(glbs->timer) {
		glbs->hw2_counter += 120;
		while(glbs->hw2_counter >= 100 && glbs->timer > 0) {
			glbs->timer--;
			glbs->hw2_counter -= 100;
		}
	}
}

//=============================================================================
// VTI detection routine (thanx, Julien Muchembled)
//
// this routine utilizes a bug in VTI: you can set even non-existing flags in
// the status register and query them later!
//=============================================================================
// VTI was the calculator emulator; on the native port there is no VTI.
static short IsVTI(void) { return 0; }

char gfx_setup()
{
	//HARDWARE_PARM_BLOCK *hw_ptr;
	short i;

	randomize();

	glbs->dbuffer = NULL;
	//screen_save = NULL;
	glbs->light_buffer = NULL;

	for(i = FIRST_AUTO_INT ; i <= LAST_AUTO_INT ; i++) {
		glbs->save_int[i - 1] = GetIntVec(AUTO_INT(i));
		SetIntVec(AUTO_INT(i), DUMMY_HANDLER);
	}

	glbs->vti = IsVTI();

	//hw_ptr = (HARDWARE_PARM_BLOCK *)FL_getHardwareParmBlock();

	//if(hw_ptr->len == 20 || glbs->vti) glbs->hw2 = FALSE;
	//else glbs->hw2 = TRUE;
	glbs->hw2 = !glbs->vti && (HW_VERSION == 2);

	//if(hw_ptr->hardwareID == 3) glbs->ti92 = FALSE;
	//else glbs->ti92 = TRUE;
	glbs->ti92 = CALCULATOR > 0;
	
	if(glbs->vti) glbs->use_flipping = FALSE;
	else glbs->use_flipping = TRUE;

	glbs->use_flipping_save = NONE;

	if(glbs->hw2) {
		SetIntVec(AUTO_INT_3, fps_timer_hw2);
		SetIntVec(AUTO_INT_1, timer_hw2);
		//EnableAutoInt3();
	} else {
		SetIntVec(AUTO_INT_1, fps_timer_hw1);
	}

	SetIntVec(AUTO_INT_5, speed_timer);

	if(glbs->ti92) {
		glbs->display_width = 192;
		glbs->display_hieght = 128;
	} else {
		glbs->display_width = 160;
		glbs->display_hieght = 100;
	}

	glbs->dbuffer = malloc(GRAYDBUFFER_SIZE);

	GrayOn();
	GrayDBufInit(glbs->dbuffer);
	glbs->light_buffer = GrayDBufGetHiddenPlane(0);
	glbs->dark_buffer = GrayDBufGetHiddenPlane(1);

	glbs->game_counter = 0;
	glbs->fps_counter = 0;
	glbs->current_fps = 0;
	glbs->frames = 0;
	glbs->seconds = 0;
	glbs->gray_adjust = 0;
	glbs->hw2_counter = 0;
	glbs->timer = 0;
	memcpy(glbs->dll_imports, dll_imports, sizeof(void *) * DLL_IMPORT_NUMBER);

	GrayAdjust(0);

	return TRUE;
}

void gfx_cleanup(void)
{
	short i;

	GrayOff();
	free(glbs->dbuffer);

	//if(screen_save) {
	//	memcpy(LCD_MEM, screen_save, LCD_SIZE);
	//	free(screen_save);
	//}

	//if(glbs->hw2 && !glbs->vti) DisableAutoInt3();

	for(i = FIRST_AUTO_INT ; i <= LAST_AUTO_INT ; i++)
		SetIntVec(AUTO_INT(i), glbs->save_int[i - 1]);

	poke(0x600017,0xB2);

	//screen_save = NULL;

	PortRestore();
}

void dll_cleanup()
{
	UnloadDLL();
	//if(dll_state != NULL) {
	//	free(dll_state);
	//	dll_state = NULL;
	//}
}

/*__DLL_interface_struct *load_dll_archive(const char *DLL_name, long ID, short major, short minor)
{
  SYM_ENTRY *entry;
  //HANDLE h;
  unsigned char *bptr = 0,*sptr = 0;
  unsigned short len,offset=0,wrongver=0;
  //unsigned long pc;
  unsigned long signature[]={__DLL_SIGNATURE,ID};
  //asm volatile("bsr 0f; 0:move.l (%%sp)+,%0":"=g"(pc));
  //if(pc<0x40000) return DLL_NOTINGHOSTSPACE;
  //if(__DLL_body_ptr) return DLL_ALREADYLOADED;
  entry=SymFindFirst(NULL,2);
  do
    {
      if(!strcmp(entry->name,DLL_name)&&entry->handle&&!entry->flags.bits.twin
        &&(entry->flags.bits.archived||!HeapGetLock(entry->handle)))
          {
            len=peek_w(bptr=HeapDeref(entry->handle))+2;
            if(!memcmp(bptr+len-5,"DLL\x00\xF8",5))
              {
                offset=0;
                for(sptr=bptr+2;(sptr<bptr+len-1)&&!offset;sptr+=2)
                  if(!memcmp(sptr,signature,8))
                    {
                      if((unsigned short)major!=((__DLL_interface_struct*)sptr)->major
                        ||(unsigned short)minor>((__DLL_interface_struct*)sptr)->minor)
                          wrongver=1;
                      else
                        offset=sptr-bptr,wrongver=0;
                    }
                  if(offset) break;
              }
          }
    } while((entry=SymFindNext()));
  if(wrongver) return NULL;//DLL_WRONGVERSION;
  if(!entry) return NULL;//DLL_NOTFOUND;

  if(!HeapLock(metdata_handle = entry->handle)) return NULL;
  metdata_offset = (unsigned long)HeapDeref(entry->handle) + 4;
  return (__DLL_interface_struct*)(sptr - 2);

  //if(!HeapLock(h=entry->handle)) return DLL_LOCKFAILED;
  //if(!(__DLL_body_ptr=malloc(len=peek_w(bptr=HeapDeref(h)+2)+2)))
  //  {
  //    HeapUnlock(h);
  //    return DLL_OUTOFMEM;
  //  }
  //memcpy(__DLL_body_ptr,bptr,len);
  //EX_patch((char*)__DLL_body_ptr+0x40000+2,(char*)__DLL_body_ptr+0x40000+len-1);
  //__DLL_interface_ptr=(__DLL_interface_struct*)((char*)__DLL_body_ptr+offset-2);
  //HeapUnlock(h);
  //return DLL_OK;
}*/

char load_metgame()
{
	UnloadDLL();
	if(LoadDLL("metgame", METROID_ID, MAJOR_VERSION, MINOR_VERSION) != DLL_OK) return FALSE;
	//if(dll_state != NULL) {
	//	dll_set_state(dll_state);
	//	dll_state = NULL;
	//}

	return TRUE;
}

char load_metsys()
{
	//dll_state = dll_get_state();
	UnloadDLL();
	if(LoadDLL("metsys", METROID_ID, MAJOR_VERSION, MINOR_VERSION) != DLL_OK) return FALSE;

	return TRUE;
}

extern void *build_metpack(void);   // Metdata.c (native port: built in memory)

char load_metpack()
{
	glbs->metpack_base = build_metpack();
	glbs->metpack_header = (METPACK_HEADER *)glbs->metpack_base;
	return (glbs->metpack_base != NULL);
}

void flipping_off()
{
	if(glbs->use_flipping_save != NONE) return;
	glbs->use_flipping_save = glbs->use_flipping;
	glbs->use_flipping = FALSE;
}

void flipping_on()
{
	if(glbs->use_flipping_save == NONE) return;
	glbs->use_flipping = glbs->use_flipping_save;
	glbs->use_flipping_save = NONE;
}

void clear_buffer(void)
{
	memset(glbs->light_buffer, 0, 3000);
	memset(glbs->dark_buffer, 0, 3000);
}

void draw_number(short x, short y, unsigned short number, char *font, char digits, void *light, void *dark)
{
	short i;
	short d = 1;
	short x_pos = x;
	char n;

	if(digits == 2) d = 10;
	if(digits == 3) d = 100;
	if(digits == 4) d = 1000;

	for(i = 0 ; i < digits ; i++) {
		n = number / d;

		//GraySprite8_OR(x_pos, y, 6,
		//	font + n * 6 + 15 * 6, font + n * 6 + 15 * 6, light, dark);
		GraySpriteX8(x_pos, y, 6, font + n * 6 + 15 * 6, font + n * 6 + 15 * 6, 1,
			light, dark, SPRITE_XOR);

		x_pos += 4;

		//number %= d;
		//d /= 10;

		if(d == 1000) {
			number -= n * 1000;
			d = 100;
		} else if(d == 100) {
			number -= n * 100;
			d = 10;
		} else if(d == 10) {
			number -= n * 10;
			d = 1;
		} else if(d == 1) {
			number -= n;
			d = 0;
		}
	}
}

// Native port: present the two 1bpp planes to the SDL window. The old
// double-buffer/plane-flip dance (for the calculator's interrupt-driven gray
// emulation) is unnecessary -- we render straight from the hidden planes.
extern void screen_present(const unsigned char *light, const unsigned char *dark);

void update_screen()
{
	screen_present(glbs->light_buffer, glbs->dark_buffer);
	glbs->frames++;
	// WASM: every frame is a chance to let the browser repaint/deliver input.
	// This covers the main loop and every loop that draws (door fades, menus).
	PLATFORM_YIELD();
}

// Emulates the calculator's periodic auto-interrupt (~70 Hz). Driven by an SDL
// timer thread (see main_sdl.c). Advances the logic clock and the countdown
// timer that delay()/fade loops busy-wait on, plus the per-second FPS/clock.
extern int platform_hz(void);

void platform_timer_tick(void)
{
	if(glbs_base == NULL) return;
	glbs_base->game_counter++;
	// On the calculator the delay()/fade countdown ran on a faster interrupt
	// (~3-4x the game-logic rate), so decrement it faster here, clamped at 0.
	if(glbs_base->timer > 0) {
		glbs_base->timer -= 4;
		if(glbs_base->timer < 0) glbs_base->timer = 0;
	}
	if(++glbs_base->fps_counter >= platform_hz()) {
		glbs_base->fps_counter = 0;
		glbs_base->seconds++;
		// Shown as a 2-digit number; clamp so an uncapped frame rate can't make
		// draw_number index past the font glyphs (out-of-bounds read).
		glbs_base->current_fps = glbs_base->frames > 99 ? 99 : glbs_base->frames;
		glbs_base->frames = 0;
	}
}

char file_exists(const char *file)
{
	HSym SymEntry;

	SymEntry = SymFind(file);
	if(!SymEntry.folder) return FALSE;
	return TRUE;
}

void *file_pointer(const char *file)
{
	HANDLE file_handle;

	if(!file_exists(file)) return NULL;

	file_handle = SymFindPtr(file,0)->handle;
	if(!HeapGetLock(file_handle)) HeapLock(file_handle);

	return HeapDeref(file_handle) + 2;
}


void error(char *txt)
{
	gfx_cleanup();
	ST_helpMsg(txt);
	//exit(0);
}

void pause()
{
	// "Press to continue": accept SEL (2nd / shoot) or DMND (diamond / jump) so
	// either action button works -- matches the touch overlay's SHOOT and JUMP.
	while(_rowread(ARROWS_ROW)&(SEL_KEY|DMND_KEY)) PLATFORM_YIELD();
	while(!(_rowread(ARROWS_ROW)&(SEL_KEY|DMND_KEY))) PLATFORM_YIELD();
	while(_rowread(ARROWS_ROW)&(SEL_KEY|DMND_KEY)) PLATFORM_YIELD();
	glbs->game_counter = 0;
}

void delay(short a)
{
	glbs->timer = a;
	while(glbs->timer) PLATFORM_YIELD();
}

