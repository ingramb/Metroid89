#include <allegro.h>
#include <stdio.h>
#include <string.h>
#include "scrollbar.h"

#define COLOR_FRONT makecol(0, 0, 192)
#define COLOR_BACK makecol(92, 92, 92)

#define COLOR_WHITE makecol(255, 255, 255)
#define COLOR_LIGHTGRAY makecol(170, 170, 170)
#define COLOR_DARKGRAY makecol(92, 92, 92)
#define COLOR_BLACK makecol(0, 0, 0)
#define COLOR_MASK makecol(255, 0, 255)

typedef struct {
	unsigned short offset;
	char x_offset;
	char y_offset;
	unsigned char width;
	unsigned char hieght;
} SPRITE_HEADER;

SPRITE_HEADER *header;

scrollbar bars;

char width_char[10];
char hieght_char[10];
char tiles_file[1024] = "";

short sprite_width = 4;
short sprite_hieght = 4;
int byte_width;
unsigned short offset;

short actual_sprite_number;

enum {
	WIDTH_LABLE,
	WIDTH_ENTRY,
	HIEGHT_LABLE,
	HIEGHT_ENTRY,
	HEADER_CHECK,
	LIGHT_CHECK,
	ALIGN_CHECK,
	NO_MASK_CHECK,
	MASK_CHECK,
	MASK3_CHECK,
	NO_INTERLEAF_CHECK,
	INTERLEAF_ROW_CHECK,
	INTERLEAF_BYTE_CHECK,
	OUTPUT_C,
	OUTPUT_ASM,
	OUTPUT_BIN,
	OUTPUT_BMP,
};

DIALOG dialog[] = {
	{d_text_proc, 0, 280, 20, 8, 255, 0, 0, 0, 0, 0, "Width:", NULL, NULL},

	{d_edit_proc, 61, 280, 38, 8, 255, 0, 0, 0, 3, 0, width_char, NULL, NULL},

	{d_text_proc, 0, 290, 20, 8, 255, 0, 0, 0, 0, 0, "Hieght:", NULL, NULL},

	{d_edit_proc, 61, 290, 38, 8, 255, 0, 0, 0, 3, 0, hieght_char, NULL, NULL},

	{d_check_proc, 100,280, 55, 8, 255, 0, 0, 0, 0, 0, "Header:", NULL, NULL},

	{d_check_proc, 100,290, 55, 8, 255, 0, 0, 0, 0, 0, "Only light:", NULL, NULL},

	{d_check_proc, 100,300, 55, 8, 255, 0, 0, 0, 0, 0, "Align:", NULL, NULL},

	{d_radio_proc, 100,320, 55, 8, 255, 0, 0, 0, 1, 0, "No Mask", NULL, NULL},

	{d_radio_proc, 100,330, 55, 8, 255, 0, 0, 0, 1, 0, "4 Color Mask", NULL, NULL},

	{d_radio_proc, 100, 340, 55, 8, 255, 0, 0, 0, 1, 0, "3 Color Mask", NULL, NULL},

	{d_radio_proc, 100,360, 55, 8, 255, 0, 0, 0, 2, 0, "No Interleaf", NULL, NULL},

	{d_radio_proc, 100,370, 55, 8, 255, 0, 0, 0, 2, 0, "Interleaf by row", NULL, NULL},

	{d_radio_proc, 100, 380, 55, 8, 255, 0, 0, 0, 2, 0, "Interleaf by byte", NULL, NULL},

	{d_radio_proc, 100,400, 55, 8, 255, 0, 0, 0, 0, 0, "Output .c", NULL, NULL},

	{d_radio_proc, 100,410, 55, 8, 255, 0, 0, 0, 0, 0, "Output .asm", NULL, NULL},

	{d_radio_proc, 100,420, 55, 8, 255, 0, 0, 0, 0, 0, "Output .bin", NULL, NULL},

	{d_radio_proc, 100,430, 55, 8, 255, 0, 0, 0, 0, 0, "Output .bmp", NULL, NULL},
		
	{NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
};

volatile int counter = 0;

void timer()
{
	counter++;
}
END_OF_FUNCTION(timer);


void grayscale_bitmap(BITMAP *new_bitmap, BITMAP *b, int brightness, int contrast)
{
	int x;
	int y;
	int bright;
	int light;
	int dark;
	int black;

	clear(new_bitmap);
	
	if(!(dialog[MASK3_CHECK].flags & D_SELECTED)) {
		dark = 255 - brightness;
		black = dark - contrast;
		light = dark + contrast;

		if(black < 0) black = 0;
		if(light > 255) light = 255;

		for(x = 0 ; x < b->w ; x++)
			for(y = 0 ; y < b->h ; y++){
				
				bright = (getr(getpixel(b, x, y)) + getb(getpixel(b, x, y))
					+ getg(getpixel(b, x, y)))/3;

				if(getpixel(b, x, y) == MASK_COLOR_16)
					putpixel(new_bitmap, x, y, COLOR_MASK);
				else if(bright < black)
					putpixel(new_bitmap, x, y, COLOR_BLACK);
				else if(bright < dark)
					putpixel(new_bitmap, x, y, COLOR_DARKGRAY);
				else if(bright < light)
					putpixel(new_bitmap, x, y, COLOR_LIGHTGRAY);
				else
					putpixel(new_bitmap, x, y, COLOR_WHITE);
			}
	} else {
		black = 255 - brightness;
		light = black + contrast / 2;
		dark = black - contrast / 2;

		if(light > 255) light = 255;
		if(dark < 0) dark = 0;

		for(x = 0 ; x < b->w ; x++)
			for(y = 0 ; y < b->h ; y++){
				
				bright = (getr(getpixel(b, x, y)) + getb(getpixel(b, x, y))
					+ getg(getpixel(b, x, y)))/3;

				if(getpixel(b, x, y) == MASK_COLOR_16)
					putpixel(new_bitmap, x, y, COLOR_WHITE);
				else if(bright < dark)
					putpixel(new_bitmap, x, y, COLOR_BLACK);
				else if(bright < light)
					putpixel(new_bitmap, x, y, COLOR_DARKGRAY);
				else
					putpixel(new_bitmap, x, y, COLOR_LIGHTGRAY);
			}
	}
}

void create_header(BITMAP *sprites, int num, int x_pos, int y_pos)
{
	int x;
	int y;
	int i;
	int plane_number = 3;
	int counter;
	char loop = true;
	int mask;

	if(dialog[MASK3_CHECK].flags & D_SELECTED) mask = COLOR_WHITE;
	else mask = COLOR_MASK;

	if(!(dialog[MASK_CHECK].flags & D_SELECTED)) plane_number--;
	if(dialog[LIGHT_CHECK].flags & D_SELECTED) plane_number--;

	y = y_pos;

	//textprintf(screen, font, 0, 380, COLOR_WHITE, "%d", num);

	while(true){
		for(x = x_pos ; x < x_pos + sprite_width ; x++)
			if(getpixel(sprites, x, y) != mask) loop = false;
		if(!loop) break;
		y++;
		if(y - y_pos == sprite_hieght){
			header[num].x_offset = -1;
			header[num].y_offset = -1;
			header[num].hieght = -1;
			header[num].width = -1;
			return;
		}
	}

	actual_sprite_number++;

	if(!(dialog[HEADER_CHECK].flags & D_SELECTED)){
		header[num].x_offset = 0;
		header[num].y_offset = 0;
		header[num].width = byte_width;
		header[num].hieght = sprite_hieght;
		offset += sprite_hieght * byte_width * plane_number;
		return;
	}

	//textout(screen, font, "1 Done", 0, 400, COLOR_WHITE);

	header[num].y_offset = y - y_pos;

	y = y_pos + sprite_hieght - 1;

	while(true){
		counter = 0;
		for(x = x_pos ; x < x_pos + sprite_width ; x++)
			if(getpixel(sprites, x, y) == mask) counter++;
		if(counter != sprite_width) break;
		if(y == y_pos) break;
		y--;
	}
	//textout(screen, font, "2 Done", 0, 400, COLOR_WHITE);

	header[num].hieght = 1 + y - y_pos - header[num].y_offset;

	//textprintf(screen, font, 0, 420, COLOR_WHITE, "y_off:%d, hieght:%d, y:%d, counter:%d", header[num].y_offset, header[num].hieght, y, counter);

	loop = true;
	x = x_pos;
	
	while(true){
		for(y = y_pos + header[num].y_offset;
			y < y_pos + header[num].y_offset + header[num].hieght ; y++)
				if(getpixel(sprites, x, y) != mask) loop = false;
		if(!loop) break;
		x++;
	}
	//textout(screen, font, "3 Done", 0, 400, COLOR_WHITE);

	header[num].x_offset = x - x_pos;

	i = byte_width - 1;
	y_pos += header[num].y_offset;

	while(true){
		if(i == 0) break;
		counter = 0;
		for(y = y_pos ; y < y_pos + header[num].hieght ; y++)
			for(x = x_pos + header[num].x_offset + i * 8 ;
				x < x_pos + header[num].x_offset + i * 8 + 8 ; x++)
					if(getpixel(sprites, x, y) == mask || x - x_pos >= sprite_width)
						counter++;
		
		if(counter != 8 * header[num].hieght) break;
		
		i--;

	}
	//textout(screen, font, "4 Done", 0, 400, COLOR_WHITE);

	header[num].width = i + 1;

	if((offset % 2) && (dialog[ALIGN_CHECK].flags & D_SELECTED)) offset++;// && (i == 1 || i == 3)) offset++;
	header[num].offset = offset;

	offset += header[num].hieght * header[num].width * plane_number;
}

void write_byte(FILE *outfile, BITMAP *sprites, int x, int y, int x_pos, int plane)
{
	int byte;
	unsigned char cbyte;
	int k;

	byte = 0;
	cbyte = 0;
	for(k = 0 ; k < 8 ; k++){
		if(x - x_pos < sprite_width) {
			if( ((getpixel(sprites, x, y) == COLOR_LIGHTGRAY ||
				getpixel(sprites, x, y) == COLOR_BLACK) && plane == 0) ||
				((getpixel(sprites, x, y) == COLOR_DARKGRAY ||
				getpixel(sprites, x, y) == COLOR_BLACK) && plane == 1) ||
				(getpixel(sprites, x, y) == COLOR_MASK && plane == 2)) {
					byte |= (1 << (7 - k));
					cbyte |= (1 << (7 - k));
			}
		} else if(plane == 2) {
			byte |= (1 << (7 - k));
			cbyte |= (1 << (7 - k));
		}
		x++;
	}
	if((dialog[OUTPUT_C].flags & D_SELECTED) || (dialog[OUTPUT_ASM].flags & D_SELECTED))
			fprintf(outfile, "0x%02x", byte);
	else if(dialog[OUTPUT_BIN].flags & D_SELECTED) fwrite(&cbyte, 1, 1, outfile);
}

void write_sprite(FILE *outfile, BITMAP *sprites, int num, int x_pos, int y_pos)
{
	int plane;
	int byte;
	//unsigned char cbyte;
	int x, y, j, k;
	int plane_number;
	char a = 0;

	if(dialog[MASK_CHECK].flags & D_SELECTED) plane_number = 3;
	else plane_number = 2;

	if(dialog[LIGHT_CHECK].flags & D_SELECTED) offset -= header[num].hieght * header[num].width;

	if((offset % 2) && (dialog[ALIGN_CHECK].flags & D_SELECTED)) {// && (header[num].width == 2 || header[num].width == 4)) {
		offset++;
		if(dialog[OUTPUT_C].flags & D_SELECTED)
				fprintf(outfile, "\n\t0x00, //alignment byte");
		else if(dialog[OUTPUT_ASM].flags & D_SELECTED)
				fprintf(outfile, "\n\tdb 0x00 ; alignment byte");
		else if(dialog[OUTPUT_BIN].flags & D_SELECTED) fwrite(&a, 1, 1, outfile);
	}
	
	if(dialog[NO_INTERLEAF_CHECK].flags & D_SELECTED) {
		for(plane = 0 ; plane < plane_number ; plane++){
			if(plane == 1 && dialog[LIGHT_CHECK].flags & D_SELECTED) continue;
			
			if(dialog[OUTPUT_C].flags & D_SELECTED) fprintf(outfile, "\n\t");
			else if(dialog[OUTPUT_ASM].flags & D_SELECTED) fprintf(outfile, "\n\tdb ");
			
			for(y = y_pos + header[num].y_offset ;
				y < y_pos + header[num].y_offset + header[num].hieght ; y++){
				
				x = x_pos + header[num].x_offset;
				
				for(j = 0 ; j < header[num].width ; j++){
					write_byte(outfile, sprites, x, y, x_pos, plane);
					
					if((dialog[OUTPUT_C].flags & D_SELECTED) ||
					((dialog[OUTPUT_ASM].flags & D_SELECTED) &&
					(j < header[num].width - 1 ||
					y < y_pos + header[num].y_offset + header[num].hieght - 1)))
							fprintf(outfile, ",");

					x += 8;
				}
			}
		}
	} else if(dialog[INTERLEAF_ROW_CHECK].flags & D_SELECTED) {
		if(dialog[OUTPUT_C].flags & D_SELECTED) fprintf(outfile, "\n\t");
		else if(dialog[OUTPUT_ASM].flags & D_SELECTED) fprintf(outfile, "\n\tdb ");

		for(y = y_pos + header[num].y_offset ;
			y < y_pos + header[num].y_offset + header[num].hieght ; y++) {
			
			for(plane = 0 ; plane < plane_number ; plane++) {
				
				if(plane == 1 && dialog[LIGHT_CHECK].flags & D_SELECTED) continue;
				
				x = x_pos + header[num].x_offset;
				for(j = 0 ; j < header[num].width ; j++) {
					write_byte(outfile, sprites, x, y, x_pos, plane);
					x += 8;
				}
			}
		}
	} else {

	}

	offset += header[num].hieght * header[num].width * plane_number;
}

unsigned short flip_short(unsigned short a)
{
	return ((a >> 8) & 0xff) | ((a & 0xff) << 8);
}

void write_header(FILE *outfile, int i)
{
	unsigned short a;
	
	if(header[i].x_offset < 0) return;
	
	if(dialog[OUTPUT_C].flags & D_SELECTED)
		fprintf(outfile, "\n\t{%d, %d, %d, %d, %d},",
			header[i].offset, header[i].x_offset, header[i].y_offset,
			header[i].width, header[i].hieght);
	else if(dialog[OUTPUT_ASM].flags & D_SELECTED)
		fprintf(outfile, "\n\tdw %d\n\tdb %d, %d, %d, %d",
			header[i].offset, header[i].x_offset, header[i].y_offset,
			header[i].width, header[i].hieght);
	else if(dialog[OUTPUT_BIN].flags & D_SELECTED) {
		a = flip_short(header[i].offset);
		fwrite(&a, sizeof(short), 1, outfile);
		fwrite(&header[i].x_offset, sizeof(char), 1, outfile);
		fwrite(&header[i].y_offset, sizeof(char), 1, outfile);
		fwrite(&header[i].width, sizeof(char), 1, outfile);
		fwrite(&header[i].hieght, sizeof(char), 1, outfile);
	}
}

void write_gfx(BITMAP *sprites)
{
	FILE *outfile = 0;
	FILE *header_file = 0;
	char file_name[500];
	char header_file_name[500];
	int sprite_number;
	int i;
	int sprite;
	int x_pos = 0;
	int y_pos = 0;
	unsigned short a;

	byte_width = sprite_width / 8;
	if(byte_width * 8 != sprite_width) byte_width++;

	sprite_number = (sprites->w / sprite_width) * (sprites->h / sprite_hieght);

	i = strlen(tiles_file) - 3;
	strncpy(file_name, tiles_file, i);
	
	if(dialog[OUTPUT_C].flags & D_SELECTED) {	
		file_name[i] = 'c';
		file_name[i + 1] = 0;
		outfile = fopen(file_name, "wt");
	} else if(dialog[OUTPUT_ASM].flags & D_SELECTED) {
		file_name[i] = 'a';
		file_name[i + 1] = 's';
		file_name[i + 2] = 'm';
		file_name[i + 3] = 0;
		outfile = fopen(file_name, "wt");
	} else if(dialog[OUTPUT_BIN].flags & D_SELECTED) {	
		file_name[i + 0] = 'b';
		file_name[i + 1] = 'i';
		file_name[i + 2] = 'n';
		file_name[i + 3] = 0;
		i--;
		strncpy(header_file_name, tiles_file, i);
		header_file_name[i + 0] = '_';
		header_file_name[i + 1] = 'h';
		header_file_name[i + 2] = 'd';
		header_file_name[i + 3] = 'r';
		header_file_name[i + 4] = '.';
		header_file_name[i + 5] = 'b';
		header_file_name[i + 6] = 'i';
		header_file_name[i + 7] = 'n';
		header_file_name[i + 8] = 0;
		outfile = fopen(file_name, "wb");
		if(dialog[HEADER_CHECK].flags & D_SELECTED)
			header_file = fopen(header_file_name, "wb");
	}

	header = new SPRITE_HEADER[sprite_number];
	offset = 0;
	actual_sprite_number = 0;

	for(i = 0 ; i < sprite_number ; i++){
		create_header(sprites, i, x_pos, y_pos);

		x_pos += sprite_width;
		if(x_pos + sprite_width > sprites->w){
			y_pos += sprite_hieght;
			x_pos = 0;
		}
	}
	
	if(dialog[HEADER_CHECK].flags & D_SELECTED) {
		if(dialog[OUTPUT_C].flags & D_SELECTED)
			fprintf(outfile, "SPRITE_HEADER header[%d] = {", actual_sprite_number);
		else if(dialog[OUTPUT_ASM].flags & D_SELECTED)
			fprintf(outfile, "header: ; %d sprites total", actual_sprite_number);
		else if(dialog[OUTPUT_BIN].flags & D_SELECTED) {
			a = flip_short(actual_sprite_number);
			fwrite(&a, sizeof(short), 1,header_file);
		}
	
		for(i = 0 ; i < sprite_number ; i++) {
			if((dialog[OUTPUT_C].flags & D_SELECTED) || (dialog[OUTPUT_ASM].flags & D_SELECTED))
				write_header(outfile, i);
			if(dialog[OUTPUT_BIN].flags & D_SELECTED) write_header(header_file, i);
		}

		if(dialog[OUTPUT_C].flags & D_SELECTED) fprintf(outfile, "\n};\n\n");
		else if(dialog[OUTPUT_ASM].flags & D_SELECTED) fprintf(outfile, "\n\n");
	}
	
	if(dialog[OUTPUT_C].flags & D_SELECTED)
		fprintf(outfile, "unsigned char gfx[%d] = {", offset);
	else if(dialog[OUTPUT_ASM].flags & D_SELECTED)
		fprintf(outfile, "gfx: ; %d bytes of data", offset);

	x_pos = 0;
	y_pos = 0;
	sprite = 0;
	offset = 0;
	
	for(i = 0 ; i < sprite_number ; i++){
		
		if(header[i].x_offset >= 0){
			if(dialog[OUTPUT_C].flags & D_SELECTED) fprintf(outfile, "\n\t//sprite %d", sprite);
			else if(dialog[OUTPUT_ASM].flags & D_SELECTED) fprintf(outfile, "\n\t; sprite %d", sprite);
			write_sprite(outfile, sprites, i, x_pos, y_pos);
			sprite++;
		}

		x_pos += sprite_width;
		if(x_pos + sprite_width > sprites->w){
			y_pos += sprite_hieght;
			x_pos = 0;
		}
	}
	if(dialog[OUTPUT_C].flags & D_SELECTED) fprintf(outfile, "\n};\n");
	else if(dialog[OUTPUT_ASM].flags & D_SELECTED) fprintf(outfile, "\n");
	fclose(outfile);
	if((dialog[OUTPUT_BIN].flags & D_SELECTED) && (dialog[HEADER_CHECK].flags & D_SELECTED))
		fclose(header_file);
	delete header;
}

void write_bmp(BITMAP *sprites)
{
	BITMAP *bmp256;
	RGB pal[256];
	RGB white = {63, 63, 63};
	RGB lightgray = {48, 48, 48};
	RGB darkgray =  {24, 24, 24};
	RGB black =  {0, 0, 0};
	RGB mask = {63, 0, 63};
	char file_name[500];
	int i = strlen(tiles_file) - 4;
	int x, y, color;

	memset(file_name, 0, 500);
	
	strncpy(file_name, tiles_file, i);
	file_name[i + 0] = '_';
	file_name[i + 1] = 'g';
	file_name[i + 2] = 'r';
	file_name[i + 3] = 'a';
	file_name[i + 4] = 'y';
	file_name[i + 5] = '.';
	file_name[i + 6] = 'b';
	file_name[i + 7] = 'm';
	file_name[i + 8] = 'p';
	file_name[i + 9] = 0;

	memset(pal, 0, sizeof(RGB) * 256);
	
	memcpy(pal + 0, &mask, sizeof(RGB));
	memcpy(pal + 1, &white, sizeof(RGB));
	memcpy(pal + 2, &lightgray, sizeof(RGB));
	memcpy(pal + 3, &darkgray, sizeof(RGB));
	memcpy(pal + 4, &black, sizeof(RGB));
	
	set_pallete(pal);
	bmp256 = create_bitmap_ex(8, sprites->w, sprites->h);
	clear(bmp256);
	
	for(x = 0 ; x < sprites->w ; x++)
		for(y = 0 ; y < sprites->h ; y++) {
			color = getpixel(sprites, x, y);
			if(color == COLOR_MASK) putpixel(bmp256, x, y, 0);
			if(color == COLOR_WHITE) putpixel(bmp256, x, y, 1);
			if(color == COLOR_LIGHTGRAY) putpixel(bmp256, x, y, 2);
			if(color == COLOR_DARKGRAY) putpixel(bmp256, x, y, 3);
			if(color == COLOR_BLACK) putpixel(bmp256, x, y, 4);
		}


	//clear(screen);
	//textout(screen, font, file_name, 0, 0, COLOR_WHITE);
	//blit(bmp256, screen, 0, 0, 20, 20, sprites->w, sprites->h);
	//while(!key[KEY_SPACE]);
	//while(key[KEY_SPACE]);

	if(save_bmp(file_name, bmp256, pal)) {
		clear(screen);
		while(!key[KEY_SPACE]);
		while(key[KEY_SPACE]);
	}


	destroy_bitmap(bmp256);
}

void main()
{
	BITMAP *tiles;
	BITMAP *tile_display;
	BITMAP *gray_tiles;
	BITMAP *gray_output;
	BITMAP *bar_buffer;
	BITMAP *big_sprite;
	DIALOG_PLAYER *player;
	RGB pallete[256];
	int brightness = 108, old_brightness = 108;
	int contrast = 50, old_contrast = 50;
	char loop = true;
	int width_input = 0;
	int hieght_input = 0;
	int width;
	int hieght;
	FILE *path_file;

	LOCK_FUNCTION(timer);
	LOCK_VARIABLE(counter);
	
	allegro_init();
	install_keyboard();
	install_timer();
	install_mouse();

	install_int_ex(timer, BPS_TO_TIMER(30));
	
	set_color_depth(16);
	set_gfx_mode(GFX_DIRECTX,640,480,0,0);
	set_color_conversion(COLORCONV_TOTAL | COLORCONV_KEEP_TRANS);

	bar_buffer = create_bitmap(320, 36);
	big_sprite = create_sub_bitmap(screen, 322, 242, 328, 238);

	bars.add_bar(0, 0, 320, 16, COLOR_FRONT, COLOR_BACK, &brightness, 255);
	bars.add_bar(0, 20, 320, 16, COLOR_FRONT, COLOR_BACK, &contrast, 255);
	
	//do_dialog(dialog, -1);

	path_file = fopen("config.ini", "r");
	if(path_file != NULL) {
		fgets(tiles_file, 1024, path_file);
		fclose(path_file);
	} else memset(tiles_file, 0, 1024);

	if(file_select_ex("SELECT TILESET FILE", tiles_file, "BMP", 80, 0, 0)) {
		
		path_file = fopen("config.ini", "w");
		fputs(tiles_file, path_file);
		fclose(path_file);

		tiles = load_bitmap(tiles_file,pallete);
		set_pallete(pallete);

		if(tiles->w > 320) width = 320;
		else width = tiles->w;
		if(tiles->h > 240) hieght = 240;
		else hieght = tiles->h;
		
		tile_display = create_bitmap(width, hieght);
		gray_tiles = create_bitmap(width, hieght);
		gray_output = create_bitmap(tiles->w, tiles->h);
		blit(tiles, tile_display, 0, 0, 0, 0, width, hieght);
		blit(tile_display, screen, 0, 0, 0, 0, width, hieght);
		
		dialog[OUTPUT_C].flags |= D_SELECTED;
		dialog[NO_MASK_CHECK].flags |= D_SELECTED;
		dialog[NO_INTERLEAF_CHECK].flags |= D_SELECTED;
		player = init_dialog(dialog, 0);

		show_mouse(screen);

		while(loop){
			grayscale_bitmap(gray_tiles, tile_display, brightness, contrast);

			scare_mouse();
			
			blit(gray_tiles, screen, 0, 0, 320, 0, width, hieght);

			clear(big_sprite);
			stretch_blit(gray_tiles, big_sprite, 0, 0, sprite_width, sprite_hieght, 0, 0,
				sprite_width * 3, sprite_hieght * 3);

			textprintf(screen, font, 0, 440, COLOR_WHITE, "Bright:%d Contrast:%d", brightness, contrast);

			unscare_mouse();

			while(old_brightness == brightness && old_contrast == contrast){
				counter = 0;

				if(key[KEY_ESC]){
					loop = false;
					break;
				}

				if(key[KEY_SPACE]) {
					grayscale_bitmap(gray_output, tiles, brightness, contrast);
					if(!(dialog[OUTPUT_BMP].flags & D_SELECTED)) write_gfx(gray_output);
					else write_bmp(gray_output);
				}

				bars.process_bars(0, 240);
				clear(bar_buffer);
				bars.display_bars(bar_buffer);
				scare_mouse();
				blit(bar_buffer, screen, 0, 0, 0, 240, 320, 36);
				update_dialog(player);
				unscare_mouse();

				width_input = atoi(width_char);
				hieght_input = atoi(hieght_char);

				if((width_input != sprite_width || hieght_input != sprite_hieght) &&
					width_input >= 2 && hieght_input >= 2 &&
					width_input <= tiles->w && hieght_input <= tiles->h){
						sprite_width = width_input;
						sprite_hieght = hieght_input;
						break;
						
				}

				while(!counter);
			}

			old_brightness = brightness;
			old_contrast = contrast;
		}

		destroy_bitmap(tiles);
		shutdown_dialog(player);
		destroy_bitmap(tile_display);
		destroy_bitmap(gray_tiles);
	}


	
	destroy_bitmap(bar_buffer);


}
END_OF_MAIN()