// C Source File
// Created 1/3/02; 4:29:35 PM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization
#define NO_AMS_CHECK
#define NO_CALC_DETECT
#define NO_EXIT_SUPPORT
#define EXECUTE_IN_GHOST_SPACE
//#define USE_TI89
//#define KERNEL_FORMAT_RELOCS

#include <tigcclib.h>         // Include All Header Files
#include "utility.h"
//#include "extgraph.h"
#include "entity.h"
#include "player.h"
#include "tiles.h"
#include "enemy.h"
#include "door.h"
#include "pipe.h"
#include "map.h"
#include "shots.h"
#include "bomb.h"
#include "animation.h"
#include "bar.h"
#include "blur.h"
#include "environment.h"
#include "block.h"
#include "powerup.h"
#include "system.h"
#include "miscgfx.h"
#include "globals.h"

//#define SPECIAL_FX_CUTOUT 10

GLOBALS *glbs_base = NULL;
GLOBALS *glbs = NULL;   // was an a5-pinned register global on m68k

void screen_draw()
{
	short i;

	//memset(glbs->light_buffer, 0, 3000);
	//memset(glbs->dark_buffer, 0, 3000);

	bg_draw();
	draw_map0();
	draw_special(0);
	draw_items();
	//if(glbs->current_fps > SPECIAL_FX_CUTOUT)
	blur_draw();
	player_draw();
	enemy_draw();
	draw_special(1);
	door_draw();
	pipe_draw();
	draw_map1(glbs->light_buffer, glbs->dark_buffer,
		glbs->tile_data, glbs->tile_list, glbs->tile_list_clipped);
	shots_draw();
	bomb_draw();
	//if(glbs->current_fps > SPECIAL_FX_CUTOUT)
		animation_draw(glbs->anim_header, glbs->anim_gfx, enemies);
	powerup_draw();
	environment_draw(glbs->water_gfx);
	bar_draw();
	if(glbs->player.gravity_counter != 0) player_draw_gravity_bar();

	for(i = 0 ; i < 6 ; i++) {
		*(char *)(glbs->light_buffer + (94 + i) * 30) = 0;
		*(char *)(glbs->dark_buffer + (94 + i) * 30) = 0;
	}
	draw_number(0, 95, glbs->current_fps, glbs->font, 2, glbs->light_buffer, glbs->dark_buffer);
}

// BENCHMARK mode (calc arg-driven FPS test) removed for the native port.
void _main(void)
{
	char screen_save[LCD_SIZE];
	void *old5;
	short repeat_loops = 0;
#ifdef BENCHMARK
	ESI argptr;
  short arg_number;
  long index;
  short frames = 0;
	short i;
#endif

	old5 = glbs;
	glbs_base = malloc(sizeof(GLOBALS));
	if(glbs_base == NULL) goto quit;
	glbs = glbs_base;
	memset(glbs, 0, sizeof(GLOBALS));

	memcpy(screen_save, LCD_MEM, LCD_SIZE);

	if(!gfx_setup()) goto quit;
	if(!load_metgame()) goto quit;
	if(!load_metpack()) goto quit;
	if(!miscgfx_setup()) goto quit;
	if(!bar_setup()) goto quit;
	if(!block_setup()) goto quit;
	if(!blur_setup()) goto quit;
	if(!animation_setup()) goto quit;
	if(!shot_setup()) goto quit;
	if(!tile_setup()) goto quit;
	if(!map_setup("mtlevel")) goto quit;
	if(!player_setup()) goto quit;
	if(!enemy_setup()) goto quit;
	if(!powerup_setup()) goto quit;

#ifdef BENCHMARK
	arg_number = ArgCount();
  InitArgPtr(argptr);

  if(arg_number == 1 && GetArgType(argptr) == POSINT_TAG) {
  	clrscr();
  	printf("sizeof(GLOBALS) - %ld", sizeof(GLOBALS));
  	pause();
	  index = GetIntArg(argptr);

	  if(index + 10 >= map_number) index = map_number - 11;

		for(i = index ; i < index + 10 ; i++) {
			set_map(i);
			focus_camera(0, 0);
			glbs->seconds = 0;
			while(glbs->seconds < 6) {
				frames++;
				screen_draw();
				update_screen();
				if(glbs->camera.x + 160 < glbs->current_map.width * 12) camera_right();
				if(glbs->camera.y + 100 < glbs->current_map.hieght * 12) camera_down();
			}
		}
		clrscr();
		printf("FRAMES:%d", frames);
		pause();
		goto quit;
	}
#endif

	load_metsys();
	
	title_screen(game_name, draw_energy, draw_time);
	
	load_metgame();

	player_init();

	if(glbs->hw2)
		set_game_speed(240);
	else
		set_game_speed(229);
		//set_game_speed(150);

	if(!game_load()) {
		short i;

		for(i = 0 ; i < zone_number ; i++) map_room_active(i, 1);
		set_map(0);
		glbs->player.items_found = 0;
		glbs->player.items_equiped = 0;
		player_set_position(272, 400);
		while(player_move(DOWN, 100));
		game_write(&glbs->save_game);
		game_write(&glbs->respawn_point);
	}

	focus_camera();

	while(!(_rowread(ESC_ROW) & ESC_KEY)) {
		while(glbs->game_counter > 0 && repeat_loops < 4) {

			player_process();
			enemy_process();
			shots_process();
			blur_process();
			bomb_process();
			animation_process();
			hole_process();
			door_process();
			pipe_process();
			powerup_process();
			map_process();
			environment_process();
			bar_process();
			if(glbs->player.hp <= 0) {
				player_death();
				respawn_player();
			}

			glbs->game_counter--;
			repeat_loops++;
		}

		repeat_loops = 0;
		glbs->game_counter = 0;
		screen_draw();
		update_screen();
	}

	game_save();
quit:
	//TEST_bg_cleanup();

	shot_cleanup();
	block_cleanup();
	blur_cleanup();
	animation_cleanup();
	bar_cleanup();
	player_cleanup();
	map_cleanup();
	bg_cleanup();
	door_cleanup();
	pipe_cleanup();
	enemy_cleanup();
	tile_cleanup();
	powerup_cleanup();
	unlock_file("tiles");
	unlock_file("samus");
	//unlock_file("mapdata");
	unlock_file("enemy");
	unlock_file("gfx");
	unlock_file("metpack");
	gfx_cleanup();
	dll_cleanup();

	memcpy(LCD_MEM, screen_save, LCD_SIZE);

	if(glbs_base != NULL) {
		free(glbs_base);
		glbs_base = NULL;
	}
	glbs = old5;
}
