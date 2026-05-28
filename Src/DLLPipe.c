// C Source File
// Created 7/28/2003; 7:52:02 PM

#include <tigcclib.h>         // Include All Header Files
#define __IN_DLL__
#include "utility.h"
#include "dllpipe.h"
#include "shots.h"
#include "player.h"
#include "dllblock.h"
#include "dllmap.h"
#include "dllclipsprites.h"
#include "globals.h"

char pipe_init(PIPE_HEADER *header, short number)
{
	short i, x, y;

	glbs->pipes = malloc(sizeof(PIPE) * number);
	if(glbs->pipes == NULL) return FALSE;
	memset(glbs->pipes, 0, sizeof(PIPE) * number);
	glbs->pipe_number = number;

	for(i = 0 ; i < number ; i++) {

		glbs->pipes[i].x = header[i].x;
		glbs->pipes[i].y = header[i].y;
		glbs->pipes[i].switch_pos = header[i].switch_pos;
		glbs->pipes[i].activation = header[i].activation;
		glbs->pipes[i].direction = header[i].direction;
		glbs->pipes[i].length = header[i].length;
		glbs->pipes[i].state = glbs->pipes[i].init_state = header[i].init_state;
		glbs->pipes[i].speed = header[i].speed;
		glbs->pipes[i].pipe_delay = header[i].auto_delay;
		glbs->pipes[i].prox = header[i].prox;
		glbs->pipes[i].toggle = header[i].toggle;

		//speed = 50, prox = 57, length = 4: speed run under, but not regular run

		if(glbs->pipes[i].activation == PIPE_AUTO)
			glbs->pipes[i].delay_counter = glbs->pipes[i].pipe_delay;

		x = glbs->pipes[i].x; y = glbs->pipes[i].y;

		if(glbs->pipes[i].state == PIPE_OPENED) {
			glbs->pipes[i].tile_x = glbs->pipes[i].x;
			glbs->pipes[i].tile_y = glbs->pipes[i].y;
			glbs->pipes[i].block_handle = block_create(x * 12, y * 12, 12, 12);
			glbs->pipes[i].current_pos = 0;
		} else {
			if(glbs->pipes[i].direction == LEFT) {
				glbs->pipes[i].tile_x = header[i].x - header[i].length;
				glbs->pipes[i].tile_y = header[i].y;
				glbs->pipes[i].block_handle =
					block_create(glbs->pipes[i].tile_x * 12, y * 12, 12 + glbs->pipes[i].length * 12, 12);
			} else if(glbs->pipes[i].direction == RIGHT) {
				glbs->pipes[i].tile_x = header[i].x + header[i].length;
				glbs->pipes[i].tile_y = header[i].y;
				glbs->pipes[i].block_handle =
					block_create(x * 12, y * 12, 12 + glbs->pipes[i].length * 12, 12);
			} else if(glbs->pipes[i].direction == UP) {
				glbs->pipes[i].tile_y = header[i].y - header[i].length;
				glbs->pipes[i].tile_x = header[i].x;
				glbs->pipes[i].block_handle =
					block_create(x * 12, glbs->pipes[i].tile_y * 12, 12, 12 + glbs->pipes[i].length * 12);
			} else if(glbs->pipes[i].direction == DOWN) {
				glbs->pipes[i].tile_y = header[i].y + header[i].length;
				glbs->pipes[i].tile_x = header[i].x;
				glbs->pipes[i].block_handle =
					block_create(x * 12, y * 12, 12, 12 + glbs->pipes[i].length * 12);
			}
			glbs->pipes[i].current_pos = header[i].length;
		}

		set_prop_data(x, y, SOLID);

		if(glbs->pipes[i].activation <= PIPE_SUPERMISSILE) {
			if(glbs->pipes[i].switch_pos == LEFT) x--;
			else if(glbs->pipes[i].switch_pos == RIGHT) x++;
			else if(glbs->pipes[i].switch_pos == UP) y--;
			else if(glbs->pipes[i].switch_pos == DOWN) y++;
			set_prop_data(x, y, SOLID);

			glbs->pipes[i].switch_x = x * 12;
			glbs->pipes[i].switch_y = y * 12;
		} else {
			glbs->pipes[i].switch_x = glbs->pipes[i].x * 12;
			glbs->pipes[i].switch_y = glbs->pipes[i].y * 12;
		}

	}

	return TRUE;
}

void pipe_reset()
{
	pipe_cleanup();
}

void pipe_cleanup()
{
	if(glbs->pipes != NULL) {
		free(glbs->pipes);
		glbs->pipes = NULL;
	}
}

void pipe_open(PIPE *p, short damage_type)
{
	if(p->activation >= PIPE_PROXIMITY ||
		(p->activation == PIPE_MISSILE && !(damage_type & DAMAGE_MISSILE)) ||
		(p->activation == PIPE_SUPERMISSILE && !(damage_type & DAMAGE_SUPERMISSILE)) ||
		(p->activation == PIPE_BOMB && !(damage_type & DAMAGE_BOMB))) return;

	if(p->toggle == PIPE_OFF) return;
	else if(p->toggle == PIPE_ONCE) p->toggle = PIPE_OFF;

	if(p->state == PIPE_CLOSED) p->state = PIPE_OPENING;
	else if(p->state == PIPE_OPENED) p->state = PIPE_CLOSING;
}

void pipe_process()
{
	short i;
	short xdir = 0;
	short ydir = 0;
	short px = glbs->player.e.x + glbs->player.e.x_off;
	short py = glbs->player.e.y + glbs->player.e.y_off;
	short pw = glbs->player.e.width;
	short ph = glbs->player.e.hieght;
	short x = 0, y = 0, w = 0, h = 0;
	char ok;

	for(i = 0 ; i < glbs->pipe_number ; i++) {

		if((glbs->pipes[i].state == PIPE_OPENED || glbs->pipes[i].state == PIPE_CLOSED) && glbs->pipes[i].toggle != PIPE_OFF) {
			if(glbs->pipes[i].delay_counter > 0) {
				glbs->pipes[i].delay_counter--;
				if(glbs->pipes[i].delay_counter <= 0) {
					if(glbs->pipes[i].state == PIPE_CLOSED) glbs->pipes[i].state = PIPE_OPENING;
					else glbs->pipes[i].state = PIPE_CLOSING;
					if(glbs->pipes[i].toggle == PIPE_ONCE) glbs->pipes[i].toggle = PIPE_OFF;
				}
			} else if(glbs->pipes[i].activation == PIPE_PROXIMITY) {
				ok = FALSE;
				if(glbs->pipes[i].direction == LEFT) {
					y = glbs->pipes[i].y * 12; w = glbs->pipes[i].length * 12 + 12; h = 12; x = glbs->pipes[i].x * 12 - w + 12;
				} else if(glbs->pipes[i].direction == RIGHT) {
					x = glbs->pipes[i].x * 12; y = glbs->pipes[i].y * 12; w = glbs->pipes[i].length * 12 + 12; h = 12;
				} else if(glbs->pipes[i].direction == UP) {
					x = glbs->pipes[i].x * 12; h = glbs->pipes[i].length * 12 + 12; w = 12; y = glbs->pipes[i].y * 12 - h + 12;
				} else if(glbs->pipes[i].direction == DOWN) {
					y = glbs->pipes[i].y * 12; x = glbs->pipes[i].x * 12; h = glbs->pipes[i].length * 12 + 12; w = 12;
				}
				if(glbs->pipes[i].direction == UP || glbs->pipes[i].direction == DOWN) {
					if(glbs->player.direction == RIGHT) px += pw - 1;
					if(py + ph > y && py < y + h && absolute(px - x) < glbs->pipes[i].prox) ok = TRUE;
				} else {
					if(px + pw > x && px < x + w && absolute(py - y) < glbs->pipes[i].prox) ok = TRUE;
				}
				if(ok) {
					if(glbs->pipes[i].state == PIPE_OPENED) glbs->pipes[i].state = PIPE_CLOSING;
					else glbs->pipes[i].state = PIPE_OPENING;
					if(glbs->pipes[i].toggle == PIPE_ONCE) glbs->pipes[i].toggle = PIPE_OFF;
				}
			}
		}

		if(glbs->pipes[i].state != PIPE_OPENING && glbs->pipes[i].state != PIPE_CLOSING) continue;

		glbs->pipes[i].speed_counter += glbs->pipes[i].speed;

		while(glbs->pipes[i].speed_counter >= 100) {

			glbs->pipes[i].speed_counter -= 100;

			xdir = ydir = 0;
			if(glbs->pipes[i].direction == LEFT) xdir = -1;
			else if(glbs->pipes[i].direction == RIGHT) xdir = 1;
			else if(glbs->pipes[i].direction == UP) ydir = -1;
			else ydir = 1; //glbs->pipes[i].direction == DOWN

			if(glbs->pipes[i].state == PIPE_OPENING) {
				if(xdir) {
					glbs->pipes[i].x_off -= xdir;
					block_stretch_horiz(glbs->pipes[i].block_handle, xdir, BLOCK_SQUISH);
				} else if(ydir) {
					glbs->pipes[i].y_off -= ydir;
					block_stretch_vert(glbs->pipes[i].block_handle, ydir, BLOCK_SQUISH);
				}
			} else {
				if(xdir) {
					glbs->pipes[i].x_off += xdir;
					block_stretch_horiz(glbs->pipes[i].block_handle, xdir, BLOCK_STRETCH);
				} else if(ydir) {
					glbs->pipes[i].y_off += ydir;
					block_stretch_vert(glbs->pipes[i].block_handle, ydir, BLOCK_STRETCH);
				}
			}

			if(glbs->pipes[i].x_off < 0 || glbs->pipes[i].y_off < 0) {
				if(xdir) { glbs->pipes[i].x_off = 11; glbs->pipes[i].tile_x--; }
				if(ydir) { glbs->pipes[i].y_off = 11; glbs->pipes[i].tile_y--; }
			}

			if(glbs->pipes[i].x_off > 11 || glbs->pipes[i].y_off > 11) {
				if(xdir) { glbs->pipes[i].x_off = 0; glbs->pipes[i].tile_x++; }
				if(ydir) { glbs->pipes[i].y_off = 0; glbs->pipes[i].tile_y++; }
			}

			if(glbs->pipes[i].x_off == 0 && glbs->pipes[i].y_off == 0) {
				if(glbs->pipes[i].state == PIPE_OPENING) {
					glbs->pipes[i].current_pos--;
					if(glbs->pipes[i].current_pos == 0) {
						glbs->pipes[i].state = PIPE_OPENED;
						if(glbs->pipes[i].activation == PIPE_AUTO ||
						glbs->pipes[i].state != glbs->pipes[i].init_state)
							glbs->pipes[i].delay_counter = glbs->pipes[i].pipe_delay;
					}
				} else {
					glbs->pipes[i].current_pos++;
					if(glbs->pipes[i].current_pos == glbs->pipes[i].length) {
						glbs->pipes[i].state = PIPE_CLOSED;
						if(glbs->pipes[i].activation == PIPE_AUTO || glbs->pipes[i].state != glbs->pipes[i].init_state)
							glbs->pipes[i].delay_counter = glbs->pipes[i].pipe_delay;
					}
				}
			}



			if(block_check(glbs->pipes[i].block_handle, px, py, pw, ph)) {
				if(glbs->pipes[i].direction == UP &&
					(glbs->pipes[i].state == PIPE_CLOSING || glbs->pipes[i].state == PIPE_CLOSED) &&
					player_move(UP, 100)) py--;
				else if(!glbs->pipes[i].reversed) {
					glbs->pipes[i].reversed = TRUE;
					if(glbs->pipes[i].state == PIPE_CLOSING || glbs->pipes[i].state == PIPE_CLOSED) {
						glbs->pipes[i].state = PIPE_OPENING; glbs->pipes[i].current_pos++;
					} else {
						glbs->pipes[i].state = PIPE_CLOSING; glbs->pipes[i].current_pos--;
					}
					if(glbs->pipes[i].toggle == PIPE_OFF) glbs->pipes[i].toggle = PIPE_ONCE;
					glbs->pipes[i].speed_counter += 100;
				}
			} else if(glbs->pipes[i].reversed) glbs->pipes[i].reversed = FALSE;

		}

	}
}

void pipe_draw()
{
	unsigned char *gfx;
	short i;
	short x, y;

	for(i = 0 ; i < glbs->pipe_number ; i++) {
		gfx = glbs->pipe_gfx;

		if(glbs->pipes[i].state != PIPE_OPENED) {
			if(glbs->pipes[i].direction == LEFT)
				for(x = glbs->pipes[i].tile_x ; x < glbs->pipes[i].x ; x++)
					GraySpriteClipX8_MASK(x * 12 + glbs->pipes[i].x_off - glbs->camera.x,
					glbs->pipes[i].tile_y * 12 - glbs->camera.y, 12, gfx + 216, 2, FALSE);
			else if(glbs->pipes[i].direction == RIGHT)
				for(x = glbs->pipes[i].x ; x <= glbs->pipes[i].tile_x ; x++)
					GraySpriteClipX8_MASK(x * 12 + glbs->pipes[i].x_off - glbs->camera.x,
					glbs->pipes[i].tile_y * 12 - glbs->camera.y, 12, gfx + 216, 2, FALSE);
			else if(glbs->pipes[i].direction == UP)
				for(y = glbs->pipes[i].tile_y ; y < glbs->pipes[i].y ; y++)
					GraySpriteClipX8_MASK(glbs->pipes[i].tile_x * 12 - glbs->camera.x,
					y * 12 + glbs->pipes[i].y_off - glbs->camera.y, 12, gfx + 144, 2, FALSE);
			else if(glbs->pipes[i].direction == DOWN)
				for(y = glbs->pipes[i].y ; y <= glbs->pipes[i].tile_y ; y++)
					GraySpriteClipX8_MASK(glbs->pipes[i].tile_x * 12 - glbs->camera.x,
					y * 12 + glbs->pipes[i].y_off - glbs->camera.y, 12, gfx + 144, 2, FALSE);
		}

		if(glbs->pipes[i].activation <= PIPE_SUPERMISSILE) {
			x = glbs->pipes[i].x * 12; y = glbs->pipes[i].y * 12;
			if(glbs->pipes[i].state >= PIPE_OPENING) gfx += 72;
			GraySpriteClipX8_MASK(x - glbs->camera.x, y - glbs->camera.y, 12, gfx, 2, FALSE);
			gfx = glbs->pipe_gfx + 72 * 4 + 72 * glbs->pipes[i].activation * 4;

			if(glbs->pipes[i].switch_pos == RIGHT) gfx += 72;
			else if(glbs->pipes[i].switch_pos == UP) gfx += 72 * 2;
			else if(glbs->pipes[i].switch_pos == DOWN) gfx += 72 * 3;

			GraySpriteClipX8_MASK(glbs->pipes[i].switch_x - glbs->camera.x, glbs->pipes[i].switch_y - glbs->camera.y,
				12, gfx, 2, FALSE);
		}
	}
}