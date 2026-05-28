// C Header File
// Created 3/12/02; 5:03:31 PM

#ifndef __BAR__
#define __BAR__

void draw_energy(short x, short y, short hp, short hp_max,
	void *light, void *dark, void *mask, void *mask2);
char bar_setup();
void bar_cleanup();
void bar_active();
void zone_name_activate();
void bar_update();
void map_update();
void bar_process();
void status_screen();
void bar_draw();
void draw_status_bar();
void draw_time(short x, short y, unsigned long s);

#endif



