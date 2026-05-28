// C Source File
// Created 7/17/02; 10:07:03 PM

char fight_run_select()
{
	



void battle(ENCOUNTER *enc)
{
	ENEMY enemies[3];
	
	char enemy_number = 0;
	short x_pos = 0;
	short a;
	register short i;
	
	for(i = 0 ; i < 3 ; i++)
		if(enc->enemies[i] >= 0) {
			memcpy(enemy_data + enc->enemies[i], enemies + i, sizeof(ENEMY));
			enemy_number++;
		}
		
	if(enemy_number == 1) x_pos = 56;
	else if(enemy_number == 2) x_pos = 28;
	else x_pos = 4;
	
	
	for(i = 0 ; i < enemy_number ; i++) enemy_draw(enemies + i, x_pos);
	
	i = -2;
	
	while(i < hero_number) {
		i++;
		if(i == -1) fight_run_select();
		else process_hero(i);
	}
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