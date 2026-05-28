// C Source File
// Created 3/8/2003; 11:19:06 AM

#define USE_TI89              // Compile for TI-89

#include <tigcclib.h>
#include "utility.h"
#include "player.h"
#include "enemy.h"
#include "shots.h"
#include "map.h"

/*DLL_INTERFACE

extern const PLAYER_ACTION player_action[];
extern const ENEMY_ACTION enemy_action[];
extern const ENEMY_DATA enemy_data[];
extern const SHOT_DATA shot_data[];
extern const SHOT_COMBO combo_data[];
extern const BACKGROUND_HEADER bg_list[];

DLL_ID METROID_ID
DLL_VERSION MAJOR_VERSION, MINOR_VERSION
DLL_EXPORTS

(void *)player_action,
(void *)enemy_action,
(void *)enemy_data,
(void *)shot_data,
(void *)combo_data,
(void *)bg_list

DLL_IMPLEMENTATION*/

METPACK_HEADER header;
unsigned short offset;
FILE *file;
/*
typedef struct {
	const ENTITY_ANIMATION *animation[6];
	char duration;
	unsigned frame_reset : 1;
	unsigned overide_action : 1;
	unsigned change_direction : 1;
	unsigned input_disabled : 1;
	unsigned char type;
	char size;
	char next_action;
} PLAYER_ACTION;
*/
/*
typedef struct {
	unsigned char frame_number;
	unsigned char repeat_frame;
	const unsigned char frames[];
} ENTITY_ANIMATION;
*/

extern const _PLAYER_ACTION player_action[];
extern const _ENEMY_ACTION enemy_action[];
extern const ENEMY_DATA enemy_data[];
extern const SHOT_DATA shot_data[];
extern const SHOT_COMBO combo_data[];
extern const _BACKGROUND_HEADER bg_list[];

typedef struct {
	void *ptr;
	unsigned short offset;
} PTR_OFFSET;

void write_player_action()
{
	PLAYER_ACTION action[ACT_END_OF_LIST];
	PTR_OFFSET convert[ACT_END_OF_LIST * 6];
	short i, a, c;

	memset(convert, 0, sizeof(PTR_OFFSET) * 6 * ACT_END_OF_LIST);
	header.player_action_offset = offset;
	offset += sizeof(PLAYER_ACTION) * ACT_END_OF_LIST;
	for(i = 0 ; i < ACT_END_OF_LIST ; i++) {
		//printf("Writing player_action[%d]...\n", i);
		action[i].duration = player_action[i].duration;
		action[i].frame_reset = player_action[i].frame_reset;
		action[i].overide_action = player_action[i].overide_action;
		action[i].change_direction = player_action[i].change_direction;
		action[i].input_disabled = player_action[i].input_disabled;
		action[i].type = player_action[i].type;
		action[i].size = player_action[i].size;
		action[i].next_action = player_action[i].next_action;
		for(a = 0 ; a < 6 ; a++) {
			for(c = 0 ; c < ACT_END_OF_LIST * 6 ; c++) {
				if(convert[c].ptr == player_action[i].animation[a]) {
					action[i].animation[a] = convert[c].offset;
					break;
				} else if(convert[c].ptr == NULL) {
					action[i].animation[a] = offset;
					convert[c].ptr = player_action[i].animation[a];
					convert[c].offset = offset;
					offset += player_action[i].animation[a]->frame_number * 2 + 2;
					break;
				} //end if
			} //end for
		} //end for
	} //end for

	fwrite(action, sizeof(PLAYER_ACTION), ACT_END_OF_LIST, file);
	for(c = 0 ; c < ACT_END_OF_LIST * 6 ; c++) {
		if(convert[c].ptr == NULL) break;
		else fwrite(convert[c].ptr, ((ENTITY_ANIMATION *)convert[c].ptr)->frame_number * 2 + 2, 1, file);
	}
	//printf("Write player_action complete");
}
/*
typedef struct {
	const ENTITY_ANIMATION *animation;
	short duration;
	short move_speed;
	char reverse_flip;

	short next_action;
} ENEMY_ACTION;
*/

void write_enemy_action()
{
	ENEMY_ACTION action[ACT_END_OF_ENEMY];
	PTR_OFFSET convert[ACT_END_OF_ENEMY];
	short i, c;

	memset(convert, 0, sizeof(PTR_OFFSET) * ACT_END_OF_ENEMY);
	header.enemy_action_offset = offset;
	offset += sizeof(ENEMY_ACTION) * ACT_END_OF_ENEMY;
	for(i = 0 ; i < ACT_END_OF_ENEMY ; i++) {
		//printf("Writing enemy_action[%d]...\n", i);
		action[i].duration = enemy_action[i].duration;
		action[i].move_speed = enemy_action[i].move_speed;
		action[i].reverse_flip = enemy_action[i].reverse_flip;
		action[i].next_action = enemy_action[i].next_action;
		for(c = 0 ; c < ACT_END_OF_ENEMY ; c++) {
			if(convert[c].ptr == enemy_action[i].animation) {
				action[i].animation = convert[c].offset;
				break;
			} else if(convert[c].ptr == NULL) {
				action[i].animation = offset;
				convert[c].ptr = enemy_action[i].animation;
				convert[c].offset = offset;
				offset += enemy_action[i].animation->frame_number * 2 + 2;
				break;
			} //end if
		} //end for
	} //end for

	fwrite(action, sizeof(ENEMY_ACTION), ACT_END_OF_ENEMY, file);
	for(c = 0 ; c < ACT_END_OF_ENEMY ; c++) {
		if(convert[c].ptr == NULL) break;
		else fwrite(convert[c].ptr, ((ENTITY_ANIMATION *)convert[c].ptr)->frame_number * 2 + 2, 1, file);
	}
	//printf("Write enemy_action complete");
}
/*
typedef struct {
	const short width;
	const short hieght;
	const short footer_hieght;
	const short scroll_x;
	const short scroll_y;
	const short auto_x;
	const short auto_y;
	const unsigned char *data;
	const unsigned char *footer;
} _BACKGROUND_HEADER;
*/
void write_bg_list()
{
	BACKGROUND_HEADER bg[BG_NUMBER];
	short i;

	header.bg_list_offset = offset;
	offset += sizeof(BACKGROUND_HEADER) * BG_NUMBER;
	for(i = 0 ; i < BG_NUMBER ; i++) {
		bg[i].width = bg_list[i].width;
		bg[i].hieght = bg_list[i].hieght;
		bg[i].footer_hieght = bg_list[i].footer_hieght;
		bg[i].scroll_x = bg_list[i].scroll_x;
		bg[i].scroll_y = bg_list[i].scroll_y;
		bg[i].auto_x = bg_list[i].auto_x;
		bg[i].auto_y = bg_list[i].auto_y;
		bg[i].data = offset;
		offset += bg[i].width * bg[i].hieght;
		if(bg_list[i].footer != NULL) {
			bg[i].footer = offset;
			offset += bg[i].footer_hieght * 10;
		} else
			bg[i].footer = 0;
	}

	fwrite(bg, sizeof(BACKGROUND_HEADER), BG_NUMBER, file);
	for(i = 0 ; i < BG_NUMBER ; i++) {
		fwrite(bg_list[i].data, bg[i].width, bg[i].hieght, file);
		if(bg_list[i].footer != NULL) fwrite(bg_list[i].footer, 10, bg[i].footer_hieght, file);
	}
}

void write_tag()
{
	fputc (0, file);
	fputs ("MTD", file);
	fputc (0, file);
	fputc (OTH_TAG, file);
	//printf("Write tag complete");
}

short check_archived (SYM_STR file)
{
	SYM_ENTRY *symptr;
	symptr = SymFindPtr(file, 0);
	return (symptr->flags.bits.archived == 1);
}

void _main()
{
	SYM_STR name = SYMSTR("metpack");
	
	//clrscr();
	//FontSetSys(F_4x6);
	
	if(check_archived(name)) EM_moveSymFromExtMem(name,HS_NULL);

	offset = sizeof(METPACK_HEADER);
	file = fopen("metpack", "wb");

	fwrite(&header, sizeof(METPACK_HEADER), 1, file);
	write_player_action();
	write_enemy_action();
	fwrite(enemy_data, sizeof(ENEMY_DATA), ENEMY_END_OF_LIST, file);
	header.enemy_data_offset = offset;
	offset += sizeof(ENEMY_DATA) * ENEMY_END_OF_LIST;
	fwrite(shot_data, sizeof(SHOT_DATA), STYPE_END_OF_LIST, file);
	header.shot_data_offset = offset;
	offset += sizeof(SHOT_DATA) * STYPE_END_OF_LIST;
	fwrite(combo_data, sizeof(SHOT_COMBO), SHOT_END_OF_LIST, file);
	header.combo_data_offset = offset;
	offset += sizeof(SHOT_COMBO) * SHOT_END_OF_LIST;
	write_bg_list();
	write_tag();

	fseek(file, 0, SEEK_SET);
	fwrite(&header, sizeof(METPACK_HEADER), 1, file);

	fclose(file);
	
	EM_moveSymToExtMem(name, HS_NULL); 
	//FontSetSys(F_6x8);
}

