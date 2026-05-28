// C Source File
// Created 2/1/2003; 1:18:01 PM

#include <tigcclib.h>         // Include All Header Files
#include "dllutility.h"
#include "globals.h"

void *light_buffer;
void *dark_buffer;

short display_width;
short display_hieght;

void **dll_imports;

void dll_init(void **ptr_list, short width, short hieght)
{
	dll_imports = ptr_list;
	display_width = width;
	display_hieght = hieght;
}
