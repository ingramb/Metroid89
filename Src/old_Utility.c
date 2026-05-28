// C Source File
// Created 10/1/01; 9:01:10 PM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization
/*
#include <tigcclib.h>         // Include All Header Files
//#include "mgray.h"

#include "extgraph.h"
#include "utility.h"
#include "system.h"

void *light_buffer = NULL;
void *dark_buffer = NULL;
void *dbuffer = NULL;

short display_width;
short display_hieght;

char vti;
char hw2;
char ti92;

char use_flipping;
char use_flipping_save;

void *screen_save = NULL;

INT_HANDLER save_int[AUTO_INT_COUNT];

volatile short counter = 0;
volatile short fps_counter = 0;
volatile short current_fps = 0;
volatile short frames = 0;
volatile short seconds = 0;
volatile short timer = 0;

DEFINE_INT_HANDLER(speed_timer)
{
	counter++;
}

DEFINE_INT_HANDLER(fps_timer_hw1)
{
	if(timer) timer--;
	fps_counter++;
	if(fps_counter ==350){
		fps_counter = 0;
		current_fps = frames;
		frames = 0;
		seconds++;
	}
}

DEFINE_INT_HANDLER(fps_timer_hw2)
{
	current_fps = frames;
	frames = 0;
	seconds++;
	ExecuteHandler(save_int[2]);
}

DEFINE_INT_HANDLER(timer_hw2)
{
	if(timer) timer--;
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
    trap   #12
    move.w #0x3000,%sr
    swap   %d0
    move.w %sr,%d0
    btst   #12,%d0
    bne    __VTI           
    swap   %d0
    move.w %d0,%sr
    moveq  #0,%d0
    rts
__VTI:
    swap   %d0
    move.w %d0,%sr
    moveq  #1,%d0
    rts
");

char gfx_setup()
{
	HARDWARE_PARM_BLOCK *hw_ptr;
	short i;
	
	randomize();
	
	dbuffer = NULL;
	screen_save = NULL;
	light_buffer = NULL;
	
	for(i = FIRST_AUTO_INT ; i <= LAST_AUTO_INT ; i++) {
		save_int[i - 1] = GetIntVec(AUTO_INT(i));
		SetIntVec(AUTO_INT(i), DUMMY_HANDLER);
	}
	
	vti = IsVTI();
	
	hw_ptr = (HARDWARE_PARM_BLOCK *)FL_getHardwareParmBlock();
	
	if(hw_ptr->len == 20 || vti) hw2 = FALSE;
	else hw2 = TRUE;
	
	if(hw_ptr->hardwareID == 3) ti92 = FALSE;
	else ti92 = TRUE;
	
	if(vti) use_flipping = FALSE;
	else use_flipping = TRUE;
	
	use_flipping_save = use_flipping;
	
	if(hw2 && !vti) {
		SetIntVec(AUTO_INT_3, fps_timer_hw2);
		SetIntVec(AUTO_INT_1, timer_hw2);
		EnableAutoInt3();
	} else
		SetIntVec(AUTO_INT_1, fps_timer_hw1);
		
	SetIntVec(AUTO_INT_5, speed_timer);
	
	if(ti92) {
		display_width = 192;
		display_hieght = 128;
	} else {
		display_width = 160;
		display_hieght = 100;
	}
	
	screen_save = malloc(LCD_SIZE);
	if(!screen_save) return FALSE;
	memcpy(screen_save, LCD_MEM, LCD_SIZE);

	dbuffer = malloc(GRAYDBUFFER_SIZE);

	GrayOn();
	GrayDBufInit(dbuffer);
	light_buffer = GrayDBufGetHiddenPlane(0);
	dark_buffer = GrayDBufGetHiddenPlane(1);
	
	counter = 0;
	fps_counter = 0;
	current_fps = 0;
	frames = 0;
	
	return TRUE;
}

void gfx_cleanup(void)
{
	short i;
	
	GrayOff();
	free(dbuffer);
	
	if(screen_save) {
		memcpy(LCD_MEM, screen_save, LCD_SIZE);
		free(screen_save);
	}
	
	if(hw2 && !vti) DisableAutoInt3();
	
	for(i = FIRST_AUTO_INT ; i <= LAST_AUTO_INT ; i++)
		SetIntVec(AUTO_INT(i), save_int[i - 1]);
		
	poke(0x600017,0xB2);
	
	screen_save = NULL;
	
	PortRestore();
}

void flipping_off()
{
	use_flipping_save = use_flipping;
	use_flipping = FALSE;
}

void flipping_on()
{
	use_flipping = use_flipping_save;
}

void clear_buffer(void)
{
	memset(light_buffer, 0, 3000);
	memset(dark_buffer, 0, 3000);
}

void update_screen(char show_fps)
{
	if(show_fps) {
		short i;
		for(i = 0 ; i < 6 ; i++) {
			*(char *)(light_buffer + (94 + i) * 30) = 0;
			*(char *)(dark_buffer + (94 + i) * 30) = 0;
		}
		draw_number(0, 95, current_fps, 2, light_buffer, dark_buffer);
	}
	
	if(!use_flipping) {
		register short i;
		register long *src1 = (long *)light_buffer;
		register long *src2 = (long *)dark_buffer;
		register long *dest1 = (long *)GrayDBufGetActivePlane(0);
		register long *dest2 = (long *)GrayDBufGetActivePlane(1);
		
		if(display_width == 192) {
			for(i = 0 ; i < display_hieght ; i++) {
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
		
			for(i = 0 ; i < display_hieght ; i++) {
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
		GrayDBufToggle();
		light_buffer = GrayDBufGetHiddenPlane(0);
		dark_buffer = GrayDBufGetHiddenPlane(1);
	}
  
	frames++;
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
}

void delay(short a)
{
	timer = a;
	while(timer);
}
*/
