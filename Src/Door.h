// C Header File
// Created 4/5/02; 7:11:02 PM

#ifndef __DOOR__
#define __DOOR__
#include <stdint.h>

typedef struct {
	short x;
	short y;
	char type;
	char level;
	short index;
	short target_map;
	short target_door;
} DOOR_HEADER;

typedef struct {
	short x;
	short y;
	short y_target;
	short index;
	char hp;
	char flash;
	char width;
	char hieght;
	char type;
	char level;
	short target_map;
	short target_door;
	char status;
	char frame;
	char frame_counter;
} DOOR;

enum door_status {
	DOOR_CLOSED,
	DOOR_OPENING,
	DOOR_CLOSING,
	DOOR_OPEN,
	ELEVATOR_GOING_DOWN,
	ELEVATOR_GOING_UP,
	ELEVATOR_COMMING_DOWN,
	ELEVATOR_COMMING_UP,
};

enum door_types {
	DOOR_NORMAL,
	DOOR_BLANK,
	DOOR_MISSILE,
	DOOR_SUPERMISSILE,
	DOOR_ENEMY_REMEMBER,
	DOOR_ENEMY_RESET,
	DOOR_BOMB_SWITCH,
	DOOR_SEALED,
	DOOR_ENEMY_OPENED,
};

#define DOOR_LEFT 0
#define DOOR_RIGHT 1
#define DOOR_UP 2
#define DOOR_DOWN 3
#define ELEVATOR_UP 4
#define ELEVATOR_DOWN 5

#define OPENED_DOORS_SIZE 15
extern DOOR *doors;
extern short door_number;
extern char opened_doors[];

char door_init(DOOR_HEADER *header, short number);
void door_reset();
void door_cleanup();
void door_bomb_switch_check();
char door_open(DOOR *door, short damage_type);
void door_process();
void door_draw();
void elevator_enter(short i);

void fade_out(uint32_t *light, uint32_t *dark, DOOR *door);
void fade_out_player(uint32_t *light, uint32_t *dark);
void fade_in(uint32_t *light, uint32_t *dark, DOOR *door);

#endif

