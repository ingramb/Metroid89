// C Source File
// Created 11/18/01; 12:31:41 PM

#define OPTIMIZE_ROM_CALLS // Use ROM Call Optimization

#define SAVE_SCREEN           // Save/Restore LCD Contents

#include <tigcclib.h>         // Include All Header Files
#include "utility.h"
#include "menubox.h"
#include "font.h"
#include "extgraph2.h"
#include "menu.h"

short _ti89;                  // Produce .89Z File

DEFINE_INT_HANDLER(menu_timer)
{
	if(menu_delay) menu_delay--;
}

// Main Function
void _main(void)
{
	int i;
	BOX *box;
	MENU *menu;
	char y_dir = 1;
	char x_dir = 1;
	
	intr_setup(menu_timer, DUMMY_HANDLER);
	gray_setup(16);
	init_small_font();
	
	clear_buffer();
	box = create_box(0,0,150,58, "KADE");
	menu = create_menu(box, 27, 3, 2);
	for(i = 0 ; i < 3 ; i++){
	add_text(menu, "jack@$$");
	add_text(menu, "Fight");
	add_text(menu, "Item");
	add_text(menu, "jack@$$");
	add_text(menu, "Fight");
	add_text(menu, "Item");
	add_text(menu, "jack@$$");
	add_text(menu, "Fight");
	add_text(menu, "Item");
	}
	for(;;){
		process_menu(menu);
		
		clear_buffer();
		display_menu(menu);
		update_screen();
	
		//box->y += y_dir;
		//box->x += x_dir;
		//if(box->y == -30 || box->y == 100) y_dir = -y_dir;
		//if(box->x == -20 || box->x == 30) x_dir = -x_dir;
	}
	
	cleanup_small_font();
	gray_cleanup();
	intr_cleanup();
	PortRestore();
}
