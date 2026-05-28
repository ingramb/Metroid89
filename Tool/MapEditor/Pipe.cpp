#include <allegro.h>
#include <string.h>
#include <stdio.h>
#include "pipe.h"

#define COLOR_BLACK makecol(0, 0, 0)
#define COLOR_WHITE makecol(255, 255, 255)
#define COLOR_RED makecol(255, 0, 0)

#define LEFT 1
#define RIGHT 2
#define UP 4
#define DOWN 8

pipe::pipe()
{
	number = 0;
	data = 0;
}

pipe::~pipe()
{
	delete data;
}

void pipe::new_pipe(short x, short y)
{
	PIPE *new_data;
	int i;
	
	for(i = 0 ; i < number ; i++)
		if(data[i].x == x && data[i].y == y) {
			modify_pipe(i);
			return;
		}

	new_data = new PIPE[number + 1];
	memset(new_data + number, 0, sizeof(PIPE));
	memcpy(new_data, data, sizeof(PIPE) * number);
	
	number++;

	new_data[number - 1].x = x;
	new_data[number - 1].y = y;
	new_data[number - 1].activation = PIPE_SHOOT;
	new_data[number - 1].direction = DOWN;
	new_data[number - 1].switch_pos = LEFT;
	new_data[number - 1].length = 4;
	new_data[number - 1].speed = 100;
	new_data[number - 1].init_state = PIPE_CLOSED;
	new_data[number - 1].toggle = 1;

	delete data;
	data = new_data;
}

enum PIPE_D {
	P_BOX,
	P_POS_LABLE,
	P_POS_LEFT,
	P_POS_RIGHT,
	P_POS_UP,
	P_POS_DOWN,
	P_ACT_LABLE,
	P_ACT_SHOOT,
	P_ACT_MISSILE,
	P_ACT_SUPERMISSILE,
	P_ACT_BOMB,
	P_ACT_PROX,
	P_ACT_AUTO,
	P_TOGGLE_CHECK,
	P_DIR_LABLE,
	P_LEFT,
	P_RIGHT,
	P_UP,
	P_DOWN,
	P_INIT_LABLE,
	P_CLOSED,
	P_OPEN,
	P_LENGTH_LABLE,
	P_LENGTH,
	P_SPEED_LABLE,
	P_SPEED,
	P_DELAY_LABLE,
	P_DELAY,
	P_PROX_LABLE,
	P_PROX,
	P_OK,
	P_NO,
	P_DELETE,
};

void pipe::modify_pipe(short i)
{
	char length_char[5];
	char speed_char[5];
	char delay_char[5];
	char prox_char[5];
	short j;

	DIALOG d[] = {
		{d_box_proc, 0, 0, 240, 170, COLOR_BLACK, COLOR_WHITE, 0, 0,	0, 0, NULL, NULL, NULL},
		
		{d_text_proc, 5, 5, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Switch Pos:", NULL, NULL},
		{d_radio_proc, 5, 15, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 1, 0, "Left", NULL, NULL},
		{d_radio_proc, 5, 25, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 1, 0, "Right", NULL, NULL},
		{d_radio_proc, 5, 35, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 1, 0, "Up", NULL, NULL},
		{d_radio_proc, 5, 45, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 1, 0, "Down", NULL, NULL},

		{d_text_proc, 5, 65, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Activation:", NULL, NULL},
		{d_radio_proc, 5, 75, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 2, 0, "Shoot", NULL, NULL},
		{d_radio_proc, 5, 85, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 2, 0, "Missile", NULL, NULL},
		{d_radio_proc, 5, 95, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 2, 0, "Supermissile", NULL, NULL},
		{d_radio_proc, 5, 105, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 2, 0, "Bomb", NULL, NULL},
		{d_radio_proc, 5, 115, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 2, 0, "Proximity", NULL, NULL},
		{d_radio_proc, 5, 125, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 2, 0, "Auto", NULL, NULL},

		{d_check_proc, 5, 135, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Toggle", NULL, NULL},

		{d_text_proc, 125, 5, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Direction:", NULL, NULL},
		{d_radio_proc, 125, 15, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 3, 0, "Left", NULL, NULL},
		{d_radio_proc, 125, 25, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 3, 0, "Right", NULL, NULL},
		{d_radio_proc, 125, 35, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 3, 0, "Up", NULL, NULL},
		{d_radio_proc, 125, 45, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 3, 0, "Down", NULL, NULL},

		{d_text_proc, 125, 65, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Initial:", NULL, NULL},
		{d_radio_proc, 125, 75, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 4, 0, "Closed", NULL, NULL},
		{d_radio_proc, 125, 85, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 4, 0, "Open", NULL, NULL},

		{d_text_proc, 125, 105, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Length:", NULL, NULL},
		{d_edit_proc, 185, 105, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 2, 0, length_char, NULL, NULL},

		{d_text_proc, 125, 115, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Speed:", NULL, NULL},
		{d_edit_proc, 185, 115, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 3, 0, speed_char, NULL, NULL},

		{d_text_proc, 125, 125, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Delay:", NULL, NULL},
		{d_edit_proc, 185, 125, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 3, 0, delay_char, NULL, NULL},

		{d_text_proc, 125, 135, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Prox:", NULL, NULL},
		{d_edit_proc, 185, 135, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 3, 0, prox_char, NULL, NULL},

		{d_button_proc,	5,	150,	55, 15,	COLOR_BLACK, COLOR_WHITE, 0, D_EXIT, 0,	0, "OK", NULL, NULL},
		{d_button_proc,	65,	150,	55,	15,	COLOR_BLACK, COLOR_WHITE, 0, D_EXIT, 0,	0, "NO", NULL, NULL},
		{d_button_proc,	125, 150, 55, 15, COLOR_BLACK, COLOR_WHITE, 0, D_EXIT, 0, 0, "DELETE", NULL, NULL},
		
		{NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
	};

	sprintf(length_char, "%d", data[i].length);
	sprintf(speed_char, "%d", data[i].speed);
	sprintf(delay_char, "%d", data[i].auto_delay);
	sprintf(prox_char, "%d", data[i].prox);
	if(data[i].switch_pos == LEFT) d[P_POS_LEFT].flags |= D_SELECTED;
	else if(data[i].switch_pos == RIGHT) d[P_POS_RIGHT].flags |= D_SELECTED;
	else if(data[i].switch_pos == UP) d[P_POS_UP].flags |= D_SELECTED;
	else if(data[i].switch_pos == DOWN) d[P_POS_DOWN].flags |= D_SELECTED;

	d[P_ACT_SHOOT + data[i].activation].flags |= D_SELECTED;
	if(data[i].toggle) d[P_TOGGLE_CHECK].flags |= D_SELECTED;

	if(data[i].direction == LEFT) d[P_LEFT].flags |= D_SELECTED;
	else if(data[i].direction == RIGHT) d[P_RIGHT].flags |= D_SELECTED;
	else if(data[i].direction == UP) d[P_UP].flags |= D_SELECTED;
	else if(data[i].direction == DOWN) d[P_DOWN].flags |= D_SELECTED;

	d[P_CLOSED + data[i].init_state].flags |= D_SELECTED;
	
	while(mouse_b);
	centre_dialog(d);
	do_dialog(d, 2);

	if(d[P_NO].flags & D_GOTFOCUS) return;
	else if(d[P_DELETE].flags & D_GOTFOCUS) {
		delete_pipe(i);
		return;
	}

	j = atoi(length_char);
	if(j >= 1) data[i].length = j;

	j = atoi(speed_char);
	if(j >= 1 && j < 256) data[i].speed = j;

	j = atoi(delay_char);
	if(j >= 0) data[i].auto_delay = j;

	j = atoi(prox_char);
	if(j >= 0 && j < 128) data[i].prox = j;

	if(d[P_TOGGLE_CHECK].flags & D_SELECTED) data[i].toggle = 1;
	else data[i].toggle = 0;

	if(d[P_POS_LEFT].flags & D_SELECTED) data[i].switch_pos = LEFT;
	else if(d[P_POS_RIGHT].flags & D_SELECTED) data[i].switch_pos = RIGHT;
	else if(d[P_POS_UP].flags & D_SELECTED) data[i].switch_pos = UP;
	else if(d[P_POS_DOWN].flags & D_SELECTED) data[i].switch_pos = DOWN;

	for(j = P_ACT_SHOOT ; j <= P_ACT_AUTO ; j++)
		if(d[j].flags & D_SELECTED) {
			data[i].activation = j - P_ACT_SHOOT;
			break;
		}

	if(d[P_LEFT].flags & D_SELECTED) data[i].direction = LEFT;
	else if(d[P_RIGHT].flags & D_SELECTED) data[i].direction = RIGHT;
	else if(d[P_UP].flags & D_SELECTED) data[i].direction = UP;
	else if(d[P_DOWN].flags & D_SELECTED) data[i].direction = DOWN;

	if(d[P_OPEN].flags & D_SELECTED) data[i].init_state = PIPE_OPEN;
	else if(d[P_CLOSED].flags & D_SELECTED) data[i].init_state = PIPE_CLOSED;

	
}

void pipe::delete_pipe(short index)
{
	PIPE *new_data;
	short i;

	new_data = new PIPE[number - 1];

	for(i = 0 ; i < index ; i++)
		memcpy(new_data + i, data + i, sizeof(PIPE));
	for(i = index + 1 ; i < number ; i++)
		memcpy(new_data + i - 1, data + i, sizeof(PIPE));

	number--;

	delete data;
	data = new_data;
}

void pipe::draw(BITMAP *buffer, short camera_x, short camera_y, tileset *pics)
{
	short i, p, d;
	short x_pos, x;
	short y_pos, y;
	short x0, y0, x1, y1;

	for(i = 0 ; i < number ; i++){
		x = x_pos = (data[i].x - camera_x) * 12;
		y = y_pos = (data[i].y - camera_y) * 12;

		draw_rle_sprite(buffer, pics->get_tile(0), x, y);

		if(data[i].activation <= PIPE_SUPERMISSILE) {

			if(data[i].activation == PIPE_SHOOT) p = 4;
			else if(data[i].activation == PIPE_MISSILE) p = 8;
			else p = 12;

			if(data[i].switch_pos == LEFT) x -= 12;
			else if(data[i].switch_pos == RIGHT) { x += 12; p++; }
			else if(data[i].switch_pos == UP) { y -= 12; p += 2; }
			else { y += 12; p += 3; }

			draw_rle_sprite(buffer, pics->get_tile(p), x, y);
		}

		if(data[i].direction == LEFT) {
			x1 = x_pos - 12;
			x0 = x_pos - data[i].length * 12;
			y0 = y1 = y_pos; p = 3; d = 12;
		} else if(data[i].direction == RIGHT) {
			x0 = x_pos + 12;
			x1 = x_pos + data[i].length * 12;
			y0 = y1 = y_pos; p = 3; d = 12;
		} else if(data[i].direction == UP) {
			y1 = y_pos - 12;
			y0 = y_pos - data[i].length * 12;
			x0 = x1 = x_pos; p = 2; d = 12;
		} else if(data[i].direction == DOWN) {
			y0 = y_pos + 12;
			y1 = y_pos + data[i].length * 12;
			x0 = x1 = x_pos; p = 2; d = 12;
		}

		for(x = x0 ; x <= x1 ; x += d) {
			for(y = y0 ; y <= y1 ; y += d) {
				if(data[i].init_state == PIPE_OPEN)
					draw_trans_rle_sprite(buffer, pics->get_tile(p), x, y);
				else
					draw_rle_sprite(buffer, pics->get_tile(p), x, y);
			}
		}



	}
}

void pipe::save_data(FILE *outfile)
{
	fwrite(&number, sizeof(short), 1, outfile);
	if(number) fwrite(data, sizeof(PIPE), number, outfile);
}

void pipe::load_data(FILE *infile)
{
	//OLD_ENEMY *old_data;
	//short i;

	if(data) delete data;
	fread(&number, sizeof(short), 1, infile);
	if(number) {
		data = new PIPE[number];
		fread(data, sizeof(PIPE), number, infile);
		
		//for(short i = 0 ; i < number ; i++) {
		//	if(data[i].type == 10) data[i].type = 0;
		//	else data[i].type++;
		//}

	} else
		data = 0;

	/*old_data = new OLD_ENEMY[number];
	fread(old_data, sizeof(OLD_ENEMY), number, infile);
	data = new pipe[number];
	for(i = 0 ; i < number ; i++) {
		data[i].type = old_data[i].type;
		data[i].subtype = old_data[i].subtype;
		data[i].gravity = old_data[i].gravity;
		data[i].direction = old_data[i].direction;
		data[i].index = -1;
		data[i].x = old_data[i].x;
		data[i].y = old_data[i].y;
	}
	delete old_data;*/
}
