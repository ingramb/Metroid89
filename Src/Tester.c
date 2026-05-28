// C Source File
// Created 3/27/02; 10:01:09 PM

#define USE_TI89              // Produce .89z File

#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

// #define SAVE_SCREEN        // Save/Restore LCD Contents

#include <tigcclib.h>         // Include All Header Files
#include <extgraph.h>
#include "utility.h"
#include "fflgui.h"

typedef struct {
	unsigned int offset;
	char x_offset;
	char y_offset;
	char width;
	char hieght;
} SPRITE_HEADER;

extern SPRITE_HEADER enemy_header[];
extern unsigned char enemy_gfx[];
extern unsigned char char_gfx[];

// Main Function
void _main(void)
{
	int i;
	int frame_size;
	BOX *box1;
	BOX *box2;
	BOX *box3;
	MENU *menu;
	
	gfx_setup(0);
	realtime_off();
	
	clear_buffer();
	
	for(i = 0 ; i < 3 ; i++) {
		frame_size = enemy_header[i].width * enemy_header[i].hieght;
		GraySpriteX8_MASK(x_buffer + 4 + 52 * i, y_buffer + 3, enemy_header[i].hieght,
			enemy_gfx + enemy_header[i].offset,
			enemy_gfx + enemy_header[i].offset + frame_size,
			enemy_gfx + enemy_header[i].offset + frame_size * 2,
			enemy_gfx + enemy_header[i].offset + frame_size * 2,
			enemy_header[i].width, light_buffer, dark_buffer);
	}
	
	box1 = box_create_text(x_buffer, y_buffer + 54, 12, 4, light_buffer, dark_buffer);
	box2 = box_create_text(x_buffer + 106, y_buffer + 54, 5, 4, light_buffer, dark_buffer);
	box3 = box_create_text(x_buffer, y_buffer, 9, 3, light_buffer, dark_buffer);
	
	GraySprite16_OR(x_buffer + 7, y_buffer + 61, 16,
		(unsigned short *)(char_gfx + 96 * 4),
		(unsigned short *)(char_gfx + 96 * 4 + 32),
		light_buffer, dark_buffer);
	
	draw_box_str(box1, 17, 0, "999");
	draw_box_str(box1, 17, 8, "/999");
	
	GraySprite16_OR(x_buffer + 56, y_buffer + 61, 16,
		(unsigned short *)(char_gfx + 96 * 4),
		(unsigned short *)(char_gfx + 96 * 4 + 32),
		light_buffer, dark_buffer);
	
	draw_box_str(box1, 66, 0, "999");
	draw_box_str(box1, 66, 8, "/999");
	
	
	GraySprite16_OR(x_buffer + 7, y_buffer + 77, 16,
		(unsigned short *)(char_gfx + 96 * 4),
		(unsigned short *)(char_gfx + 96 * 4 + 32),
		light_buffer, dark_buffer);
	
	draw_box_str(box1, 17, 16, "999");
	draw_box_str(box1, 17, 24, "/999");
	
	GraySprite16_OR(x_buffer + 56, y_buffer + 77, 16,
		(unsigned short *)(char_gfx + 96 * 4),
		(unsigned short *)(char_gfx + 96 * 4 + 32),
		light_buffer, dark_buffer);
	
	draw_box_str(box1, 66, 16, "999");
	draw_box_str(box1, 66, 24, "/999");
	
	draw_box_str(box3, 0, 0, "Giant   6");
	draw_box_str(box3, 0, 8, "Ninja   3");
	draw_box_str(box3, 0, 16, "Warrior 5");
	
	menu = menu_create(box2, 2, 0);
	menu_add_choice(menu, "Fight");
	menu_add_choice(menu, "Run");
	menu_draw(menu);
	
	while(!menu_process(menu))
		update_screen(FALSE);
	
	box_kill(box1);
	box_kill(box2);
	box_kill(box3);
	menu_kill(menu);
	
	gfx_cleanup();
}
