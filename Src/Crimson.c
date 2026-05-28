// C Source File
// Created 9/9/2002; 7:00:08 PM

#define USE_TI89              // Compile for TI-89

// #define OPTIMIZE_ROM_CALLS // Use ROM Call Optimization

// #define SAVE_SCREEN        // Save/Restore LCD Contents

#include <tigcclib.h>         // Include All Header Files
#include <extgraph.h>
#include "utility.h"
#include "soldier.h"

// Main Function
void _main(void)
{
	SOLDIER s;
	
	gfx_setup();
	particle_setup();
	
	soldier_init(&s, 80, 50, 0, 100);
	
	while(!(_rowread(ESC_ROW) & ESC_KEY)) {
		clear_buffer();
		soldier_process(&s);
		soldier_draw(&s);
		update_screen(TRUE);
	}
	
	gfx_cleanup();
	particle_cleanup();
}
