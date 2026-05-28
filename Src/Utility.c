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

#define _unpack_decompress_ ((__attribute__((__stkparm__)) unsigned short(*)(unsigned char*,unsigned char*))(unsigned short[]){\
0x48e7,0x7ffa,0x4fef,0xffe8,0x206f,0x0050,0x0c28,0x0054,0x0002,0x6608,\
0x0c28,0x0050,0x0003,0x6708,0x303c,0x00fc,0x6000,0x0264,0x4286,0x1c28,\
0x0006,0x4247,0x1e28,0x0009,0x4245,0x1a28,0x000a,0x5345,0x4240,0x1028,\
0x000c,0x3c40,0x7201,0xeb69,0x3f41,0x000e,0x7002,0xeb68,0x3f40,0x000c,\
0x7208,0x9245,0x3f41,0x000a,0x7008,0x9047,0x3840,0x303c,0x00f9,0x0c47,\
0x0008,0x6200,0x021e,0x4240,0x1028,0x000b,0xb06f,0x000e,0x660c,0x0c45,\
0x0004,0x6306,0x0c45,0x0007,0x6308,0x303c,0x00fa,0x6000,0x01fe,0x7204,\
0xb24e,0x6408,0x303c,0x00fb,0x6000,0x01f0,0x43e8,0x000f,0x2f49,0x0010,\
0x266f,0x0054,0x43fa,0x01ea,0x2f49,0x0006,0x226f,0x0010,0x4280,0x1011,\
0x41f0,0x0810,0x226f,0x0006,0x2288,0x41fa,0x01d4,0x2f48,0x0002,0x30bc,\
0x0080,0x3606,0x4a47,0x670c,0x3f07,0x41fa,0x0274,0x4e90,0x3600,0x548f,\
0x3043,0xbc88,0x6600,0x018c,0x3f05,0x45fa,0x021a,0x4e92,0x3800,0x426f,\
0x0002,0x548f,0x0c44,0x0001,0x6754,0x3f05,0x4e92,0x3600,0x5343,0x302f,\
0x000e,0x5540,0x548f,0xb043,0x6618,0x0c44,0x0003,0x6300,0x0170,0x45fa,\
0x017e,0x4e92,0x3f40,0x0000,0x4e92,0x6000,0x0120,0x300e,0x6710,0x3f0e,\
0x41fa,0x021a,0x4e90,0x320e,0xe36b,0x8640,0x548f,0x41fa,0x015a,0x4e90,\
0x0a40,0x00ff,0x3203,0xe149,0x8240,0x6000,0x00fe,0x226f,0x0006,0x2051,\
0x4240,0x1010,0x206f,0x0002,0x3210,0xc041,0x6700,0x00ca,0xe219,0x6402,\
0x5291,0x3081,0x2051,0x4240,0x1010,0xc041,0x6630,0x3001,0xe218,0x6402,\
0x5291,0x226f,0x0002,0x3280,0x3f07,0x45fa,0x01c0,0x4e92,0x3600,0x3f0c,\
0x4e92,0x320c,0x48c1,0xe3ae,0x8c00,0x16c6,0x4286,0x3c03,0x588f,0x6000,\
0xff26,0x3001,0xe218,0x6402,0x5291,0x206f,0x0002,0x3080,0x3f05,0x4e92,\
0x3600,0x548f,0xb66f,0x000e,0x6522,0x3f2f,0x000a,0x41fa,0x0180,0x4e90,\
0x966f,0x0010,0x322f,0x000c,0xe36b,0x8640,0x3f05,0x4e92,0x5340,0xe148,\
0x8640,0x588f,0x3f05,0x4e92,0x3800,0x548f,0x0c44,0x001f,0x6210,0x4280,\
0x3004,0x206f,0x0010,0x1230,0x0800,0x4881,0x6016,0x3f3c,0x0003,0x41fa,\
0x0140,0x4e90,0x3204,0x0641,0xffe0,0xe749,0x8240,0x548f,0x4242,0xb642,\
0x6500,0xfeac,0x16c1,0x5242,0xb642,0x64f8,0x6000,0xfea0,0x3001,0x226f,\
0x0006,0xe218,0x6402,0x5291,0x206f,0x0002,0x3080,0x41fa,0x0056,0x4e90,\
0x3200,0x0a41,0x00ff,0x4242,0xb842,0x6500,0xfe7a,0x4280,0x3001,0x204b,\
0x91c0,0x102f,0x0001,0xd028,0xffff,0x16c0,0x5242,0xb842,0x64e8,0x6000,\
0xfe5e,0x3f0c,0x41fa,0x00d6,0x4e90,0x320c,0xe36b,0x8600,0x16c3,0x548f,\
0x6000,0xfe48,0x4240,0x4fef,0x0018,0x4cdf,0x5ffe,0x4e75,0x0000,0x0000,\
0x0000,0x41fa,0xfff8,0x2250,0x4240,0x1019,0x2089,0x323a,0xfff0,0x0c01,\
0x0080,0x6602,0x4e75,0xe148,0x1011,0x0c01,0x0008,0x6212,0x6630,0x0240,\
0x0fff,0xe848,0x4e75,0x0240,0x7fff,0xee48,0x4e75,0x0c01,0x0020,0x62f2,\
0x6508,0x0240,0x3fff,0xec48,0x4e75,0x0240,0x1fff,0xea48,0x4e75,0x0240,\
0x07ff,0xe648,0x4e75,0x0c01,0x0002,0x62f2,0x6608,0x0240,0x03ff,0xe448,\
0x4e75,0x0240,0x01ff,0xe248,0x4e75,0x2f03,0x4281,0x302f,0x0008,0x207a,\
0xff84,0x343a,0xff84,0xb041,0x6312,0x3602,0xc610,0xe21a,0x6402,0x5288,\
0x4a03,0x6704,0x5241,0x60ea,0x2608,0x41fa,0xff64,0x2083,0x41fa,0xff62,\
0x3082,0x2043,0x7601,0xe36b,0x3f01,0x6108,0x544f,0x8043,0x261f,0x4e75,\
0x4240,0x322f,0x0004,0x6730,0x207a,0xff3e,0x2243,0x343a,0xff3c,0x5341,\
0xe348,0x3602,0xc610,0x6702,0x5200,0xe21a,0x6402,0x5288,0x51c9,0xffee,\
0x2609,0x43fa,0xff20,0x3282,0x43fa,0xff16,0x2288,0x2243,0x4e75})

short __attribute__((__stkparm__)) OldUnpackBuffer(unsigned char *src, unsigned char *dest) {
    return _unpack_decompress_(src,dest);
}

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
extern short IsVTI(void);
asm("
IsVTI:
    trap   #12         /* enter supervisor mode. returns old (%sr) in %d0.w   */
    move.w #0x3000,%sr /* set a non-existing flag in %sr (but keep s-flag !!) */
    swap   %d0         /* save %d0.w content in upper part of %d0             */
    move.w %sr,%d0     /* get %sr content and check for non-existing flag     */
    btst   #12,%d0     /* this non-existing flag can only be set on the VTI   */
    bne    __VTI
    swap   %d0         /* restore old %sr content and return 0                */
    move.w %d0,%sr
    moveq  #0,%d0
    rts
__VTI:
    swap   %d0         /* restore old %sr content and return 1                */
    move.w %d0,%sr
    moveq  #1,%d0
    rts
");

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

char load_metpack()
{
	glbs->metpack_base = file_pointer(SYMSTR("metpack"));
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

void update_screen()
{
	if(!glbs->use_flipping) {
		register short i;
		register long *src1 = (long *)glbs->light_buffer;
		register long *src2 = (long *)glbs->dark_buffer;
		register long *dest1 = (long *)GrayDBufGetActivePlane(0);
		register long *dest2 = (long *)GrayDBufGetActivePlane(1);

		if(glbs->display_width == 192) {
			for(i = 0 ; i < glbs->display_hieght ; i++) {
				*dest1++ = *src1++; *dest2++ = *src2++;
				*dest1++ = *src1++; *dest2++ = *src2++;
				*dest1++ = *src1++; *dest2++ = *src2++;
				*dest1++ = *src1++; *dest2++ = *src2++;
				*dest1++ = *src1++; *dest2++ = *src2++;
				*dest1++ = *src1++; *dest2++ = *src2++;

				(char *)dest1 += 6; (char *)dest2 += 6;
				(char *)src1 += 6; (char *)src2 += 6;
			}

		} else {

			for(i = 0 ; i < glbs->display_hieght ; i++) {
				*dest1++ = *src1++; *dest2++ = *src2++;
				*dest1++ = *src1++; *dest2++ = *src2++;
				*dest1++ = *src1++; *dest2++ = *src2++;
				*dest1++ = *src1++; *dest2++ = *src2++;
				*dest1++ = *src1++; *dest2++ = *src2++;

				(char *)dest1 += 10; (char *)dest2 += 10;
				(char *)src1 += 10; (char *)src2 += 10;
			}
		}

	} else {
		GrayDBufToggleSync();
		glbs->light_buffer = GrayDBufGetHiddenPlane(0);
		glbs->dark_buffer = GrayDBufGetHiddenPlane(1);
	}

	glbs->frames++;
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
	while(_rowread(ARROWS_ROW)&SEL_KEY);
	while(!(_rowread(ARROWS_ROW)&SEL_KEY));
	while(_rowread(ARROWS_ROW)&SEL_KEY);
	glbs->game_counter = 0;
}

void delay(short a)
{
	glbs->timer = a;
	while(glbs->timer);
}

