#include <allegro.h>
#include <stdio.h>
#include "tiles.h"
#include "enemy.h"
#include "doors.h"
#include "screen.h"
#include "map.h"

#define AUTO_SAVE_DELAY 500

#define COLOR_WHITE makecol(255, 255, 255)
#define COLOR_SELECTED makecol(255, 0, 0)

#define PIPE_X 250
#define PIPE_Y 490

#define	AREA_NONE 0
#define	AREA_MAP 1
#define AREA_TILES 2
#define AREA_ENEMY 3
#define AREA_DOORS 4

BITMAP *buffer;
BITMAP *map_area;
BITMAP *tiles_area;
BITMAP *enemy_area;
map *maps;

unsigned char *tile_grid = 0;
short grid_width = 0;
short grid_hieght = 0;
short stile_x0 = 0;
short stile_y0 = 0;
short stile_x1 = 0;
short stile_y1 = 0;

volatile short speed_counter = 0;
volatile long auto_timer = 0;

void increment_speed_counter()
{
	speed_counter++;
}
END_OF_FUNCTION(increment_speed_counter);

void increment_auto_timer()
{
	auto_timer++;
}
END_OF_FUNCTION(increment_auto_timer);

void reset_grid()
{
	if(tile_grid) delete tile_grid;
	tile_grid = new unsigned char[1];
	tile_grid[0] = 0;
	grid_width = 1;
	grid_hieght = 1;
	stile_x0 = 0;
	stile_y0 = 0;
	stile_x1 = 0;
	stile_y1 = 0;
}

void draw_enemy_select(short selected)
{
	int i;

	for(i = 0 ; i <= ENEMY_TYPE_MAX ; i++){
		if(i == selected)
			rect(buffer, i * 30, 583, i * 30 + 28, 599, COLOR_SELECTED);
		else
			rect(buffer, i * 30, 583, i * 30 + 28, 599, COLOR_WHITE);
		
		if(i == ENEMY_TYPE_MAX)
			textprintf(buffer, font, i * 30 + 3, 586, COLOR_WHITE, "X", i);
		else
			textprintf(buffer, font, i * 30 + 3, 586, COLOR_WHITE, "%d", i);
	}
}


void main_loop()
{
	static tileset *pipe_pics;
	static tileset *enemy_pics;
	BITMAP *bmp;
	RGB pal[256];

	char file_name[1024] = "c:/calculator/metroid gfx/";
	FILE *file;
	short tile_x = 0;
	short tile_y = 0;
	short selected_enemy = -1;
	short selected_door = -1;
	bool pipe_selected = false;
	short tile_pos = 0;
	short mouse_area = 0;
	short x, y;

	char a_pressed = false;
	char minus_pressed = false;
	char plus_pressed = false;
	char space_pressed = false;
	char home_pressed = false;
	char enter_pressed = false;
	char button_down = false;
	char rbutton_down = false;
	char selection_start = false;
	char tiles_selected = true;
	char read_tiles = false;

	pipe_pics = new tileset();
	bmp = load_bitmap("c:/calculator/metroid gfx/pipestuff.bmp", pal);
	pipe_pics->load_tiles(bmp, 12, 12);
	destroy_bitmap(bmp);

	enemy_pics = new tileset();
	bmp = load_bitmap("c:/calculator/metroid gfx/enemy_pics.bmp", pal);
	enemy_pics->load_tiles(bmp, 16, 16);
	destroy_bitmap(bmp);

	reset_grid();

	while(!key[KEY_ESC]){

		while(speed_counter > 0) {
			
			mouse_area = AREA_NONE;
			
			if(mouse_x > 0 && mouse_y > 0 && mouse_x < 481 && mouse_y < 481){
				tile_x = (mouse_x - 1) / 12;
				tile_y = (mouse_y - 1) / 12;
				mouse_area = AREA_MAP;
			} else if(mouse_x >= 560 && mouse_y < 552) {
				tile_x = (mouse_x - 560) / 24;
				tile_y = mouse_y / 24;
				tile_pos = tile_y * 10 + tile_x;
				if(tile_pos < maps->get_tile_number())
					mouse_area = AREA_TILES;
			} else if(mouse_x < 480 && mouse_y > 568) {
				tile_x = mouse_x / 16;
				tile_y = (mouse_y - 569) / 16;
				tile_pos = tile_y * 28 + tile_x;
				if(tile_pos <= ENEMY_TYPE_MAX)
					mouse_area = AREA_ENEMY;
			} else if(mouse_x >= 464 && mouse_y > 552) {
				tile_x = (mouse_x - 464) / 48;
				mouse_area = AREA_DOORS;
			}

			if(mouse_b & 1){
				if(mouse_area == AREA_MAP){
					if(tiles_selected) {
						for(x = 0 ; x < grid_width ; x++)
							for(y = 0 ; y < grid_hieght ; y++)
								maps->set_tile(tile_grid[y * grid_width + x], maps->camera_x + tile_x + x, maps->camera_y + tile_y + y);
					} else if(selected_enemy >= 0 && !button_down){
						if(selected_enemy == ENEMY_TYPE_MAX)
							maps->modify_enemy(tile_x, tile_y);
						else
							maps->new_enemy(tile_x, tile_y, selected_enemy);
						button_down = true;
					} else if(selected_door >= 0 && !button_down) {
						if(selected_door < 5)
							maps->new_door(tile_x, tile_y, selected_door);
						else
							maps->door_properties(tile_x, tile_y);

						button_down = true;
					} else if(pipe_selected && !button_down) {
						maps->new_pipe(tile_x, tile_y);
						button_down = true;
					}
				} else if(mouse_area == AREA_TILES && !button_down){
					stile_x0 = tile_x;
					stile_y0 = tile_y;
					selected_enemy = -1;
					selected_door = -1;
					button_down = true;
					selection_start = true;
					tiles_selected = false;
					pipe_selected = false;
				} else if(mouse_area == AREA_ENEMY){
					selected_enemy = tile_pos;
					tiles_selected = false;
					selected_door = -1;
					pipe_selected = false;
				} else if(mouse_area == AREA_DOORS){
					selected_door = tile_x;
					tiles_selected = false;
					selected_enemy = -1;
					pipe_selected = false;
				} else if(mouse_x >= PIPE_X && mouse_y >= PIPE_Y &&
					mouse_x <= PIPE_X + 11 && mouse_y <= PIPE_Y + 11) {
						pipe_selected = true;
						tiles_selected = false;
						selected_door = -1;
						selected_enemy = -1;
				}
			} else if(button_down) {
				button_down = false;
				if(mouse_area == AREA_TILES && selection_start) {
					stile_x1 = tile_x;
					stile_y1 = tile_y;
					grid_width = stile_x1 - stile_x0 + 1;
					grid_hieght = stile_y1 - stile_y0 + 1;
					if(grid_width > 0 && grid_hieght > 0) {
						if(tile_grid) delete tile_grid;
						tile_grid = new unsigned char[grid_width * grid_hieght];
						for(x = 0 ; x < grid_width ; x++) {
							for(y = 0 ; y < grid_hieght ; y++) {
								tile_grid[y * grid_width + x] = (y + stile_y0) * 10 + x + stile_x0;
							}
						}
					} else reset_grid();
					tiles_selected = true;
					selection_start = false;
					read_tiles = false;
				} else if(selection_start) {
					selection_start = false;
					read_tiles = false;
					tiles_selected = true;
					reset_grid();
				}
			}

			if(mouse_b & 2){
				if(mouse_area == AREA_MAP && !selection_start) {
					stile_x0 = tile_x + maps->camera_x;
					stile_y0 = tile_y + maps->camera_y;
					selection_start = true;
					rbutton_down = true;
					read_tiles = true;
					tiles_selected = false;
					selected_enemy = -1;
					selected_door = -1;
					pipe_selected = false;
				}
			} else if(rbutton_down) {
				if(selection_start && read_tiles) {
					stile_x1 = tile_x + maps->camera_x;
					stile_y1 = tile_y + maps->camera_y;
					grid_width = stile_x1 - stile_x0 + 1;
					grid_hieght = stile_y1 - stile_y0 + 1;
					if(grid_width > SCREEN_WIDTH) grid_width = SCREEN_WIDTH;
					if(grid_hieght > SCREEN_HIEGHT) grid_hieght = SCREEN_HIEGHT;
					if(grid_width > 0 && grid_hieght > 0) {
						if(tile_grid) delete tile_grid;
						tile_grid = new unsigned char[grid_width * grid_hieght];
						for(x = 0 ; x < grid_width ; x++) {
							for(y = 0 ; y < grid_hieght ; y++) {
								tile_grid[y * grid_width + x] = maps->read_tile(stile_x0 + x, stile_y0 + y);
							}
						}
					} else {
						reset_grid();
						read_tiles = false;
					}
					tiles_selected = true;
					selection_start = false;
				}
				rbutton_down = false;
			}

			
			if(key[KEY_A] && !a_pressed) {
				maps->new_map();
				reset_grid();
				a_pressed = true;
			} else if(!key[KEY_A])
				a_pressed = false;
			if(key[KEY_EQUALS] && !plus_pressed) {
				maps->next_map();
				reset_grid();
				plus_pressed = true;
			} else if(!key[KEY_EQUALS])
				plus_pressed = false;
			if(key[KEY_MINUS] && !minus_pressed) {
				maps->prev_map();
				reset_grid();
				minus_pressed = true;
			} else if(!key[KEY_MINUS])
				minus_pressed = false;
			if(key[KEY_SPACE] && !space_pressed) {
				maps->change_tile_mode();
				space_pressed = true;
				reset_grid();
			} else if(!key[KEY_SPACE])
				space_pressed = false;
			if(key[KEY_Z])
				maps->set_zone_names();
			
			if(key[KEY_ENTER] && mouse_area == AREA_MAP) {
				maps->select_map(tile_x, tile_y);
				reset_grid();
			}
			if(key[KEY_HOME]) maps->center_view();

			if(key[KEY_S] && mouse_area == AREA_MAP)
				maps->screen_properties(maps->camera_x + tile_x, maps->camera_y + tile_y);
			

			if(key[KEY_LEFT]) maps->camera_left();
			if(key[KEY_RIGHT]) maps->camera_right();
			if(key[KEY_UP]) maps->camera_up();
			if(key[KEY_DOWN]) maps->camera_down();


			if(key[KEY_F1]){
				if(file_select_ex("Save As", file_name, "mmp", 300, 640, 480)){
					file = fopen(file_name, "wb");
					maps->save_data(file);
					fclose(file);
				}
			}
			if(key[KEY_F2]){
				if(file_select_ex("Open", file_name, "mmp", 300, 640, 480)){
					file = fopen(file_name, "rb");
					maps->load_data(file);
					fclose(file);
					reset_grid();
				}
			}
			if(key[KEY_F3]){
				if(file_select_ex("Export", file_name, "89y", 300, 640, 480)){
					//file = fopen(file_name, "wb");
					maps->export_data(file_name);
					//fclose(file);
				}
			}
			
			if(key[KEY_F5]) maps->map_properties();

			if(auto_timer >= AUTO_SAVE_DELAY) {
				maps->auto_save();
				auto_timer = 0;
			}

			speed_counter--;

		}

		clear_to_color(buffer, makecol(120, 100, 100));

		rect(buffer, 0, 0, 481, 481, COLOR_WHITE);

		maps->draw_maps(map_area, enemy_pics, pipe_pics);
		maps->draw_tiles(tiles_area, 10, -1);

		if(mouse_area == AREA_MAP) {
			if(tiles_selected) {
				rect(map_area, tile_x * 12, tile_y * 12, (tile_x + grid_width) * 12 - 1, (tile_y + grid_hieght) * 12 - 1, COLOR_WHITE);
				for(x = 0 ; x < grid_width ; x++)
					for(y = 0 ; y < grid_hieght ; y++)
						draw_trans_rle_sprite(map_area, maps->get_tile(tile_grid[y * grid_width + x]), (tile_x + x) * 12, (tile_y + y) * 12);
			} else if(read_tiles && selection_start)
				rect(map_area, (stile_x0 - maps->camera_x) * 12, (stile_y0 - maps->camera_y) * 12, tile_x * 12 + 11, tile_y * 12 + 11, COLOR_WHITE);
			else
				rect(map_area, tile_x * 12, tile_y * 12, tile_x * 12 + 11, tile_y * 12 + 11, COLOR_WHITE);
			textprintf(buffer, font, 0, 550, COLOR_WHITE, "Xpos: %d, Ypos: %d",
				tile_x + maps->camera_x, tile_y + maps->camera_y);
		}
		
		if(mouse_area == AREA_TILES) {
			if(selection_start)
				rect(tiles_area, stile_x0 * 24, stile_y0 * 24, tile_x * 24 + 23, tile_y * 24 + 23, COLOR_WHITE);
			else
				rect(tiles_area, tile_x * 24, tile_y * 24, tile_x * 24 + 23, tile_y * 24 + 23, COLOR_WHITE);
		}

		if(tiles_selected && !selection_start && !read_tiles)
			rect(tiles_area, stile_x0 * 24, stile_y0 * 24, stile_x1 * 24 + 23, stile_y1 * 24 + 23, COLOR_SELECTED);

		textprintf(buffer, font, 0, 500, COLOR_WHITE, "MAP: %d/%d", maps->current_map + 1, maps->map_number);
		maps->printdim(buffer, 0, 510, COLOR_WHITE);

		enemy_pics->draw_small(enemy_area, 28, selected_enemy, 0, 0);
		maps->draw_doors_select(buffer, 464, 552, selected_door);

		draw_rle_sprite(buffer, pipe_pics->get_tile(0), PIPE_X, PIPE_Y);
		if(pipe_selected) rect(buffer, PIPE_X, PIPE_Y, PIPE_X + 11, PIPE_Y + 11, COLOR_SELECTED);

		draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
		vsync();
		blit(buffer, screen, 0, 0, 0, 0, 800, 600);
	}

	maps->auto_save();
	delete enemy_pics;
	delete pipe_pics;
	if(tile_grid) delete tile_grid;
}

void main()
{
	allegro_init();
	install_keyboard();
	install_timer();
	install_mouse();

	//set_mouse_speed(20000000, 20000000);

	set_color_depth(16);
	set_gfx_mode(GFX_DIRECTX,800,600,0,0);
	set_color_conversion(COLORCONV_TOTAL | COLORCONV_KEEP_TRANS);
	set_trans_blender(0, 0, 0, 128);
	text_mode(-1);

	buffer = create_bitmap(800, 600);
	map_area = create_sub_bitmap(buffer, 1, 1, 480, 480);
	tiles_area = create_sub_bitmap(buffer, 560, 0, 240, 600);
	enemy_area = create_sub_bitmap(buffer, 0, 568, 800, 600);

	LOCK_VARIABLE(speed_counter);
	LOCK_FUNCTION(increment_speed_counter);
	LOCK_VARIABLE(auto_timer);
	LOCK_FUNCTION(increment_auto_timer);

	install_int_ex(increment_speed_counter, BPS_TO_TIMER(30));
	install_int_ex(increment_auto_timer, BPS_TO_TIMER(1));
	
	maps = new map();
	
	main_loop();

	destroy_bitmap(buffer);
}
END_OF_MAIN()