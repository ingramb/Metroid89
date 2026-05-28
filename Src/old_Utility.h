// C Header File
// Created 10/2/01; 8:47:35 PM

#ifndef __UTILITY__
#define __UTILITY__

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

#define light_buffer _DLL_glbvar(void *, LIGHT_BUFFER)
#define dark_buffer _DLL_glbvar(void *, DARK_BUFFER)
#define game_counter _DLL_glbvar(volatile short, GAME_COUNTER)
#define current_fps _DLL_glbvar(volatile short, CURRENT_FPS)
#define timer _DLL_glbvar(volatile short, TIMER)
#define seconds _DLL_glbvar(volatile unsigned long, SECONDS)
#define vti _DLL_glbvar(char, VTI)
#define hw2 _DLL_glbvar(char, HW2)
#define ti92 _DLL_glbvar(char, IS_TI92)
#define display_width _DLL_glbvar(short, DISPLAY_WIDTH)
#define display_hieght _DLL_glbvar(short, DISPLAY_HIEGHT)

#define gfx_setup _DLL_call(char, (), GFX_SETUP)
#define gfx_cleanup _DLL_call(void, (), GFX_CLEANUP)
#define flipping_off _DLL_call(void, (), FLIPPING_OFF)
#define flipping_on _DLL_call(void, (), FLIPPING_ON)
#define clear_buffer _DLL_call(void, (), CLEAR_BUFFER)
#define update_screen _DLL_call(void, (), UPDATE_SCREEN)
#define pause _DLL_call(void, (), PAUSE)
#define delay _DLL_call(void, (short), DELAY)
#define file_exists _DLL_call(char, (const char *), FILE_EXISTS)
#define file_pointer _DLL_call(void *, (const char *), FILE_POINTER)
#define error _DLL_call(void, (const char *), ERROR)

#endif

