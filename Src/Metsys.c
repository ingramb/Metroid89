// C Source File
// Created 2/1/2003; 3:23:07 PM

//#define USE_TI89              // Compile for TI-89
#define NO_CALC_CHECK
#define NO_EXIT_SUPPORT

#include "dll.h"
#include "dllexports.h"

DLL_INTERFACE

#include "dllsystem.h"
#include "dlltitle.h"

DLL_ID METROID_ID
DLL_VERSION MAJOR_VERSION, MINOR_VERSION
DLL_EXPORTS
	string_width,
	draw_string,
	draw_string_ex,
	message,
	draw_border,
	menu_create,
	menu_add_choice,
	menu_process,
	title_screen
DLL_IMPLEMENTATION
