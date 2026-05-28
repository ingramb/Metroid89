// C Source File
// Created 7/22/2003; 3:40:52 PM

/*
//returns TRUE if move is ok, FALSE otherwise
char check_move_horiz(ENTITY *e, char xdir)
{
	short tile_x;
	short y;
	char ok = TRUE;
	unsigned char data;

	if(xdir < 0) tile_x = e->tile_x0 - 1;
	else tile_x = e->tile_x1 + 1;

	for(y = e->tile_y0 ; y <= e->tile_y1 ; y++) {
		data = glbs->current_map.prop_data[y * glbs->current_map.width + tile_x];
		if(data >= SOLID && data < SLOPE_RIGHT_UP) { ok = FALSE; break; }
	}

	data = glbs->current_map.prop_data[e->tile_y1 * glbs->current_map.width + tile_x];
	if(xdir > 0 && data == e->slope_right_up) e->slope_type = e->slope_right_up;
	else if(xdir < 0 && data == e->slope_left_up) e->slope_type = e->slope_left_up;


char entity_move_internal(ENTITY *e, char xdir, char ydir, short speed)
{
	short old_x = e->x;
	short old_y = e->y;
	char data;

	if(xdir) e->x_speed_counter += speed;
	if(ydir) e->y_speed_counter += speed;
	e->x_moved = e->y_moved = 0;

	if(xdir > 0) {
		while(e->x_speed_counter >= 100) {

			if(e->tile_xoff1 != 11 || check_move_horiz(e, xdir)) {
				e->x++;
				e->tile_xoff0++;
				e->tile_xoff1++;
				if(e->tile_xoff0 == 12) { e->tile_xoff0 = 0; e->tile_x0++; }
				if(e->tile_xoff1 == 12) { e->tile_xoff1 = 0; e->tile_x1++; }
			}

			if(e->slope_type == e->slope_right_up) {
				e->y++;
				e->tile_yoff0++;
				e->tile_yoff1++;
				if(e->tile_yoff0 == 12) { e->tile_yoff0 = 0; e->tile_y0++; }
				if(e->tile_yoff1 == 12) { e->tile_yoff1 = 0; e->tile_y1++; }
			} else if(e->slope_type == e->slope_left_up) {
				e->y--;
				e->tile_yoff0--;
				e->tile_yoff1--;
				if(e->tile_yoff0 < 0) { e->tile_yoff0 = 11; e->tile_y0--; }
				if(e->tile_yoff1 < 0) { e->tile_yoff1 = 11; e->tile_y1--; }
			}

			e->x_speed_counter -= 100;
		}
	} else if(xdir < 0) {
		while(e->x_speed_counter >= 100) {


			e->x_speed_counter -= 100;
		}
	}

	if(ydir > 0) {
		while(e->y_speed_counter >= 100) {


			e->y_speed_counter -= 100;
		}
	} else if(ydir < 0) {
		while(e->y_speed_counter >= 100) {


			e->y_speed_counter -= 100;
		}
	}

	return (e->x != old_x || e->y != old_y);
}
*/