#include <allegro.h>
#include <string.h>
#include <stdio.h>
#include "G:\Documents and Settings\Ben Ingram\My Documents\Visual Studio Projects\MarioBattle\SpriteSet.h"

typedef struct {
	unsigned char xpos[7];
	unsigned char ypos[7];
	char spike_frame;
} TAIL_FRAME;

#define COLOR_WHITE makecol(255, 255, 255)
#define COLOR_BLACK makecol(0, 0, 0)
#define COLOR_RED makecol(250, 30, 30)

#define X_RES 640
#define Y_RES 480
#define RIDLEY_BASE 8
#define TAIL_BASE 18
#define SPIKE_BASE 21
#define R_BUFFER_WIDTH 256
#define R_BUFFER_HIEGHT 256
#define I_BUFFER_WIDTH 300
#define I_BUFFER_HIEGHT 100
#define D_XPOS 2
#define D_YPOS (Y_RES - 80)
#define D_COLOR1 makecol(0, 0, 0)
#define D_COLOR2 makecol(255, 255, 255)

#define R_XPOS ((R_BUFFER_WIDTH - 72) / 2)
#define R_YPOS ((R_BUFFER_HIEGHT - 84) / 2)

enum {
	NEXT_FRAME,
	PREV_FRAME,
	INSERT_FRAME,
	DEL_FRAME,

	TAIL_BUTTON0,
	TAIL_BUTTON1,
	TAIL_BUTTON2,
	TAIL_BUTTON3,
	TAIL_BUTTON4,
	TAIL_BUTTON5,
	TAIL_BUTTON6,
	TAIL_BUTTON7,

	LABLE0,
	START_INPUT,
	LABLE1,
	END_INPUT,
	LABLE2,
	DELAY_INPUT,
	LOOP_CHECK,
	ANIM_BUTTON,
	
};

char start_input[5] = "0";
char end_input[5] = "0";
char delay_input[5] = "100";
char start_lable[] = "Start &Animation";
char end_lable[] = "End &Animation";

DIALOG d[] = {
	{d_button_proc, 0, 0, 100, 16, 0, 0, KEY_N, 0, 0, 0, "&Next Frame", NULL, NULL},
	{d_button_proc, 0, 18, 100, 16, 0, 0, KEY_P, 0, 0, 0, "&Prev Frame", NULL, NULL},
	{d_button_proc, 0, 36, 100, 16, 0, 0, KEY_I, 0, 0, 0, "&Insert Frame", NULL, NULL},
	{d_button_proc, 0, 54, 100, 16, 0, 0, KEY_D, 0, 0, 0, "&Delete Frame", NULL, NULL},

	{d_radio_proc, 238, 0, 50, 8, 0, 0, 0, 0, 0, 0, "N", NULL, NULL},
	{d_radio_proc, 238, 10, 50, 8, 0, 0, 0, 0, 0, 0, "NE", NULL, NULL},
	{d_radio_proc, 238, 20, 50, 8, 0, 0, 0, 0, 0, 0, "E", NULL, NULL},
	{d_radio_proc, 238, 30, 50, 8, 0, 0, 0, 0, 0, 0, "SE", NULL, NULL},
	{d_radio_proc, 238, 40, 50, 8, 0, 0, 0, 0, 0, 0, "S", NULL, NULL},
	{d_radio_proc, 238, 50, 50, 8, 0, 0, 0, 0, 0, 0, "SW", NULL, NULL},
	{d_radio_proc, 238, 60, 50, 8, 0, 0, 0, 0, 0, 0, "W", NULL, NULL},
	{d_radio_proc, 238, 70, 50, 8, 0, 0, 0, 0, 0, 0, "NW", NULL, NULL},
	
	{d_text_proc, 104, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Start:", NULL, NULL},
	{d_edit_proc, 154, 0, 40, 16, 0, 0, 0, 0, 3, 0, start_input, NULL, NULL},

	{d_text_proc, 104, 12, 0, 0, 0, 0, 0, 0, 0, 0, "End:", NULL, NULL},
	{d_edit_proc, 154, 12, 40, 16, 0, 0, 0, 0, 3, 0, end_input, NULL, NULL},

	{d_text_proc, 104, 24, 0, 0, 0, 0, 0, 0, 0, 0, "Delay:", NULL, NULL},
	{d_edit_proc, 154, 24, 60, 16, 0, 0, 0, 0, 4, 0, delay_input, NULL, NULL},

	{d_check_proc, 104, 36, 50, 10, 0, 0, 0, 0, 0, 0, "Loop", NULL, NULL},

	{d_button_proc, 104, 48, 130, 16, 0, 0, KEY_A, 0, 0, 0, start_lable, NULL, NULL},

	{NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
};

volatile short timer;
void game_timer()
{
	timer++;
}
END_OF_FUNCTION(game_timer);

short flip_short(short a)
{
	return ((a >> 8) & 0xff) | ((a & 0xff) << 8);
}

TAIL_FRAME *new_frame(TAIL_FRAME *frames, short number)
{
	TAIL_FRAME *new_frames = new TAIL_FRAME[number + 1];
	short i;
	
	if(frames != 0) {
		memcpy(new_frames, frames, sizeof(TAIL_FRAME) * number);
		delete frames;
	}
	frames = new_frames;

	if(number == 0) {
		for(i = 0 ; i < 7 ; i++) {
			frames[number].xpos[i] = R_XPOS + 52;
			frames[number].ypos[i] = R_YPOS + i * 12 + 65;
		}
		frames[number].spike_frame = 4;
	} else {
		for(i = 0 ; i < 7 ; i++) {
			frames[number].xpos[i] = frames[number - 1].xpos[i];
			frames[number].ypos[i] = frames[number - 1].ypos[i];
		}
		frames[number].spike_frame = frames[number - 1].spike_frame;
	}

	return frames;
}

TAIL_FRAME *delete_frame(TAIL_FRAME *frames, short f, short number)
{
	TAIL_FRAME *new_frames = new TAIL_FRAME[number - 1];
	short i;

	for(i = 0 ; i < number ; i++) {
		if(i < f) memcpy(new_frames + i, frames + i, sizeof(TAIL_FRAME));
		if(i > f) memcpy(new_frames + i - 1, frames + i, sizeof(TAIL_FRAME));
	}
	delete frames;
	frames = new_frames;

	return frames;
}

TAIL_FRAME *insert_frame(TAIL_FRAME *frames, short f, short number)
{
	TAIL_FRAME *new_frames = new TAIL_FRAME[number + 1];
	short i;

	for(i = 0 ; i < number + 1 ; i++) {
		if(i <= f) memcpy(new_frames + i, frames + i, sizeof(TAIL_FRAME));
		if(i == f + 1) memcpy(new_frames + i, new_frames + i - 1, sizeof(TAIL_FRAME));
		if(i > f + 1) memcpy(new_frames + i, frames + i - 1, sizeof(TAIL_FRAME));
	}

	delete frames;
	frames = new_frames;

	return frames;
}

void set_spike_frame(char spike_frame)
{
	object_message(&d[TAIL_BUTTON0 + spike_frame], MSG_KEY, 0);
}

int main(void)
{
	DIALOG_PLAYER *d_player;
	BITMAP *r_buffer;
	BITMAP *i_buffer;
	SpriteSet *ridley;
	
	allegro_init();
	set_color_depth(16);
	set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, X_RES, Y_RES, 0, 0);
	install_timer();
	install_keyboard();
	install_mouse();
	LOCK_VARIABLE(timer);
	LOCK_FUNCTION(game_timer);
	install_int(game_timer, 1);
	text_mode(-1);

	ridley = new SpriteSet("G:/calculator/metroid gfx/ridley_small.bmp", 72, 84);
	r_buffer = create_bitmap(R_BUFFER_WIDTH, R_BUFFER_HIEGHT);
	i_buffer = create_bitmap(I_BUFFER_WIDTH, I_BUFFER_HIEGHT);
	set_dialog_color(d, D_COLOR1, D_COLOR2);
	position_dialog(d, D_XPOS, D_YPOS);
	d_player = init_dialog(d, -1);
	clear_to_color(screen, COLOR_WHITE);
	rect(screen, 0, 0, R_BUFFER_WIDTH + 1, R_BUFFER_HIEGHT + 1, COLOR_RED);
	show_mouse(screen);
//=====================================================================================================
//MAIN PROGRAM STARTS HERE
//=====================================================================================================
	TAIL_FRAME *frames = new_frame(0, 0);
	short frame_number = 1;
	short f = 0, i, j, segment = -1, x, y;
	short anim_start, anim_end, anim_delay;
	char anim_direction;
	bool ridley_redraw = true;
	bool mouse_down = false;
	bool animating = false;
	char file_name[500] = "G:/calculator/tt/bin/ridley_tail.rtail";

	set_spike_frame(frames[f].spike_frame);

	while(!key[KEY_ESC]) {
		if(ridley_redraw) {
			clear_to_color(r_buffer, COLOR_WHITE);
			draw_rle_sprite(r_buffer, ridley->get_sprite(RIDLEY_BASE), R_XPOS, R_YPOS);
			rect(r_buffer, R_XPOS, R_YPOS, R_XPOS + 71, R_YPOS + 83, COLOR_RED);
			for(i = 0 ; i < 7 ; i++) {
				if(i < 6) draw_rle_sprite(r_buffer, ridley->get_sprite(TAIL_BASE + i / 2),
					frames[f].xpos[i], frames[f].ypos[i]);
				else draw_rle_sprite(r_buffer, ridley->get_sprite(SPIKE_BASE + frames[f].spike_frame),
					frames[f].xpos[i], frames[f].ypos[i]);
				if(segment == i) rect(r_buffer, frames[f].xpos[i], frames[f].ypos[i],
					frames[f].xpos[i] + 11, frames[f].ypos[i] + 11, COLOR_RED);
			}

			clear_to_color(i_buffer, COLOR_WHITE);
			textprintf(i_buffer, font, 0, 0, COLOR_BLACK, "Frame:%d/%d", f + 1, frame_number);
			for(i = 0 ; i < 7 ; i++) {
				textprintf(i_buffer, font, 10, 10 + i * 10, COLOR_BLACK, "X:%d, Y:%d",
					frames[f].xpos[i], frames[f].ypos[i]);
			}

			scare_mouse();
			blit(r_buffer, screen, 0, 0, 1, 1, R_BUFFER_WIDTH, R_BUFFER_HIEGHT);
			blit(i_buffer, screen, 0, 0, R_BUFFER_WIDTH + 10, 0, I_BUFFER_WIDTH, I_BUFFER_HIEGHT);
			unscare_mouse();
			ridley_redraw = false;
		}

		if((mouse_b & 1) && segment == -1 && !animating) {
			for(i = 6 ; i >= 0 ; i--) {
				x = frames[f].xpos[i] + 1;
				y = frames[f].ypos[i] + 1;
				if(mouse_x >= x && mouse_y >= y && mouse_x <= x + 11 && mouse_y <= y + 11) {
					segment = i;
					show_mouse(NULL);
					set_mouse_range(1, 1, R_BUFFER_WIDTH, R_BUFFER_HIEGHT);
					position_mouse(frames[f].xpos[segment] + 1, frames[f].ypos[segment] + 1);
					break;
				}
			}
		} else if(!(mouse_b & 1) && segment != -1) {
			segment = -1;
			show_mouse(screen);
			set_mouse_range(0, 0, X_RES - 1, Y_RES - 1);
			ridley_redraw = true;
		}

		if(segment != -1) {
			frames[f].xpos[segment] = mouse_x - 1;
			frames[f].ypos[segment] = mouse_y - 1;
			ridley_redraw = true;
		}

		if(animating) {
			if(timer >= anim_delay) {
				f += anim_direction;
				if(f > anim_end && !(d[LOOP_CHECK].flags & D_SELECTED)) f = anim_start;
				if(d[LOOP_CHECK].flags & D_SELECTED) {
					if(f == anim_end) anim_direction = -1;
					else if(f == anim_start) anim_direction = 1;
				}
				timer = 0;
				ridley_redraw = true;
				set_spike_frame(frames[f].spike_frame);
			}
		}

		
		update_dialog(d_player);
		if(d[NEXT_FRAME].flags & D_SELECTED) {
			d[NEXT_FRAME].flags &= ~D_SELECTED;
			d[NEXT_FRAME].flags |= D_DIRTY;
			f++;
			if(f >= frame_number) {
				frames = new_frame(frames, frame_number);
				frame_number++;
			}
			ridley_redraw = true;
			set_spike_frame(frames[f].spike_frame);
		}
		if(d[PREV_FRAME].flags & D_SELECTED) {
			d[PREV_FRAME].flags &= ~D_SELECTED;
			d[PREV_FRAME].flags |= D_DIRTY;
			f--;
			if(f < 0) f = frame_number - 1;
			ridley_redraw = true;
			set_spike_frame(frames[f].spike_frame);
		}
		if(d[DEL_FRAME].flags & D_SELECTED) {
			d[DEL_FRAME].flags &= ~D_SELECTED;
			d[DEL_FRAME].flags |= D_DIRTY;
			if(frame_number != 1) {
				frames = delete_frame(frames, f, frame_number);
				frame_number--;
				if(f >= frame_number) f--;
				ridley_redraw = true;
				set_spike_frame(frames[f].spike_frame);
			}
		}
		if(d[INSERT_FRAME].flags & D_SELECTED) {
			d[INSERT_FRAME].flags &= ~D_SELECTED;
			d[INSERT_FRAME].flags |= D_DIRTY;
			frames = insert_frame(frames, f, frame_number);
			frame_number++;
			f++;
			ridley_redraw = true;
			set_spike_frame(frames[f].spike_frame);
		}
		if(d[ANIM_BUTTON].flags & D_SELECTED) {
			d[ANIM_BUTTON].flags &= ~D_SELECTED;
			d[ANIM_BUTTON].flags |= D_DIRTY;
			animating = !animating;
			if(animating) {
				d[ANIM_BUTTON].dp = end_lable;
				anim_start = atoi(start_input);
				anim_end = atoi(end_input);
				anim_delay = atoi(delay_input);
				if(anim_delay < 0) anim_delay = 0;
				if(anim_start < 0) anim_start = 0;
				if(anim_end < 0) anim_end = 0;
				if(anim_start >= frame_number) anim_start = frame_number - 1;
				if(anim_end >= frame_number) anim_end = frame_number - 1;
				if(anim_start > anim_end) anim_start = anim_end;
				for(i = 0 ; i < ANIM_BUTTON ; i++) {
					d[i].flags |= D_DISABLED;
					d[i].flags |= D_DIRTY;
				}
				f = anim_start;
				anim_direction = 1;
				timer = 0;
				ridley_redraw = true;
				set_spike_frame(frames[f].spike_frame);
			} else {
				d[ANIM_BUTTON].dp = start_lable;
				for(i = 0 ; i < ANIM_BUTTON ; i++) {
					d[i].flags &= ~D_DISABLED;
					d[i].flags |= D_DIRTY;
				}
			}
		}

		j = frames[f].spike_frame;
		for(i = TAIL_BUTTON0 ; i <= TAIL_BUTTON7 ; i++) {
			if(d[i].flags & D_SELECTED) frames[f].spike_frame = i - TAIL_BUTTON0;
		}
		if(j != frames[f].spike_frame) ridley_redraw = true;
			
		if(key[KEY_F1]) {
			if(file_select_ex("Save As", file_name, "rtail", 500, 0, 0)) {
				FILE *file = fopen(file_name, "wb");
				short fnum = flip_short(frame_number);
				fwrite(&fnum, sizeof(short), 1, file);
				fwrite(frames, sizeof(TAIL_FRAME), frame_number, file);
				fclose(file);
			}
		}

		if(key[KEY_F2]) {
			if(file_select_ex("Open", file_name, "rtail", 500, 0, 0)) {
				FILE *file = fopen(file_name, "rb");
				short fnum;
				fread(&fnum, sizeof(short), 1, file);
				frame_number = flip_short(fnum);
				if(frames) delete frames;
				frames = new TAIL_FRAME[frame_number];
				fread(frames, sizeof(TAIL_FRAME), frame_number, file);
				fclose(file);
				f = 0;
				ridley_redraw = true;
				set_spike_frame(frames[f].spike_frame);
			}
		}	
	}

	shutdown_dialog(d_player);
	destroy_bitmap(r_buffer);
	
	return 0;
}
END_OF_MAIN();