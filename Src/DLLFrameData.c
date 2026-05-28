// C Source File
// Created 3/8/2003; 11:23:44 AM

#include <tigcclib.h>
#include "player.h"

#define NONE -1

const ENTITY_ANIMATION face_front = {1, 0, {0, 1}};
const ENTITY_ANIMATION left_turn = {3, 0, {1, 4, 2, 4, 3, 4}};
const ENTITY_ANIMATION right_turn = {3, 0, {3, 4, 2, 4, 1, 4}};
const ENTITY_ANIMATION duck_left_turn = {3, 0, {4, 4, 5, 4, 6, 4}};
const ENTITY_ANIMATION duck_right_turn = {3, 0, {6, 4, 5, 4, 4, 4}};
const ENTITY_ANIMATION air_left_turn = {3, 0, {7, 4, 8, 4, 9, 4}};
const ENTITY_ANIMATION air_right_turn = {3, 0, {9, 4, 8, 4, 7, 4}};
const ENTITY_ANIMATION stand = {4, 0, {0, 15, 1, 15, 2, 15, 1, 15}};
const ENTITY_ANIMATION hurt = {1, 0, {6, 1}};
const ENTITY_ANIMATION stand_gun_up = {1, 0, {3, 1}};
const ENTITY_ANIMATION stand_gun_diag_up = {1, 0, {4, 1}};
const ENTITY_ANIMATION stand_gun_diag_down = {1, 0, {5, 1}};
const ENTITY_ANIMATION run = {10, 0, {14, 4, 15, 4, 16, 4, 17, 4, 18, 4, 19, 4, 20, 4, 21, 4, 22, 4, 23, 4}};
const ENTITY_ANIMATION duck_change = {1, 0, {7, 1}};
const ENTITY_ANIMATION duck = {4, 0, {8, 15, 9, 15, 10, 15, 9, 15}};
const ENTITY_ANIMATION duck_gun_up = {1, 0, {11, 1}};
const ENTITY_ANIMATION duck_gun_diag_up = {1, 0, {12, 1}};
const ENTITY_ANIMATION duck_gun_diag_down = {1, 0, {13, 1}};
const ENTITY_ANIMATION ball_down = {2, 0, {98, 5, 99, 5}};
const ENTITY_ANIMATION ball_up = {2, 0, {99, 5, 98, 5}};
const ENTITY_ANIMATION ball = {8, 0, {100, 5, 101, 5, 102, 5, 103, 5, 104, 5, 105, 5, 106, 5, 107, 5}};
const ENTITY_ANIMATION fall = {5, 4, {93, 5, 94, 5, 95, 48, 96, 5, 97, 1}};
const ENTITY_ANIMATION land = {1, 0, {64, 1}};
const ENTITY_ANIMATION flip_change = {1, 0, {78, 1}};
const ENTITY_ANIMATION jump_up = {3, 2, {64, 5, 65, 5, 66, 1}};
const ENTITY_ANIMATION jump_down = {4, 3, {67, 5, 68, 5, 69, 5, 70, 1}};
const ENTITY_ANIMATION jump_gun_out = {1, 0, {71, 1}};
const ENTITY_ANIMATION jump_gun_up = {1, 0, {72, 1}};
const ENTITY_ANIMATION jump_gun_down = {1, 0, {73, 1}};
const ENTITY_ANIMATION jump_gun_diag_up = {1, 0, {74, 1}};
const ENTITY_ANIMATION jump_gun_diag_down = {1, 0, {75, 1}};
const ENTITY_ANIMATION turbo_jump_start = {7, 6, {64, 5, 65, 5, 66, 5, 67, 5, 68, 5, 69, 5, 70, 15}};
const ENTITY_ANIMATION turbo_jump = {1, 0, {76, 1}};
const ENTITY_ANIMATION turbo_charge = {1, 0, {77, 1}};
const ENTITY_ANIMATION turbo_end = {11, 0, {76, 30, 78, 5, 79, 3, 80, 2, 81, 3, 82, 2, 83, 3, 84, 2, 85, 3, 86, 2, 79, 3}};
const ENTITY_ANIMATION flip_jump = {8, 0, {79, 4, 80, 3, 81, 4, 82, 3, 83, 4, 84, 3, 85, 4, 86, 3}};
const ENTITY_ANIMATION space_jump = {4, 0, {87, 3, 88, 3, 89, 3, 90, 3}};
const ENTITY_ANIMATION wall_kick_ready = {1, 0, {91, 10}};
const ENTITY_ANIMATION wall_kick = {1, 0, {92, 1}};

const _PLAYER_ACTION src_player_action[ACT_END_OF_LIST] = {

	{{&face_front, &face_front, &face_front, &face_front, &face_front, &face_front},
		0, FALSE, FALSE, FALSE, TRUE, TYPE_STAND, SIZE_STAND, NONE},

	{{&face_front, &face_front, &face_front, &face_front, &face_front, &face_front},
		0, FALSE, FALSE, FALSE, TRUE, TYPE_STAND, SIZE_STAND, NONE},

	{{&stand, &stand, &stand, &stand, &stand, &stand},
		0, FALSE, FALSE, TRUE, TRUE, TYPE_STAND, SIZE_STAND, NONE},

	{{&left_turn, &left_turn, &left_turn, &left_turn, &left_turn, &left_turn},
		11, FALSE, TRUE, FALSE, FALSE, 0, 0,	NONE},

	{{&right_turn, &right_turn, &right_turn, &right_turn, &right_turn, &right_turn},
		11, FALSE, TRUE, FALSE, FALSE, 0, 0, NONE},

	{{&duck_left_turn, &duck_left_turn, &duck_left_turn, &duck_left_turn, &duck_left_turn, &duck_left_turn},
		11, FALSE, TRUE, FALSE, FALSE, 0, 0, NONE},

	{{&duck_right_turn, &duck_right_turn, &duck_right_turn, &duck_right_turn, &duck_right_turn, &duck_right_turn},
		11, FALSE, TRUE, FALSE, FALSE, 0, 0, NONE},

	{{&air_left_turn, &air_left_turn, &air_left_turn, &air_left_turn, &air_left_turn, &air_left_turn},
		11, FALSE, TRUE, FALSE, FALSE, 0, 0, NONE},

	{{&air_right_turn, &air_right_turn, &air_right_turn, &air_right_turn, &air_right_turn, &air_right_turn},
		11, FALSE, TRUE, FALSE, FALSE, 0, 0, NONE},

	{{&stand, &stand, &stand_gun_up, &stand, &stand_gun_diag_up, &stand_gun_diag_down},
		0, TRUE, FALSE, TRUE, FALSE, TYPE_STAND, SIZE_STAND, NONE},

	{{&hurt, &hurt, &hurt, &hurt, &hurt, &hurt},
		1, FALSE, FALSE, TRUE, TRUE, TYPE_STAND | TYPE_BOUNCE | TYPE_JUMP_UP, SIZE_STAND, ACT_STAND},

	{{&run, &run, &run, &run, &run, &run},
		1, FALSE, FALSE, TRUE, FALSE, TYPE_STAND, SIZE_STAND, ACT_STAND},

	{{&duck_change, &duck_change, &duck_change, &duck_change, &duck_change, &duck_change},
		6, FALSE, FALSE, TRUE, FALSE, TYPE_STAND, SIZE_DUCK, ACT_DUCK},

	{{&duck, &duck, &duck_gun_up, &duck, &duck_gun_diag_up, &duck_gun_diag_down},
		0, TRUE,	FALSE, TRUE, FALSE, TYPE_STAND, SIZE_DUCK, NONE},

	{{&duck_change, &duck_change, &duck_change, &duck_change, &duck_change, &duck_change},
		6, FALSE, FALSE, TRUE, FALSE, TYPE_STAND, SIZE_DUCK, ACT_STAND},

	{{&ball_down, &ball_down, &ball_down, &ball_down, &ball_down, &ball_down},
		9, FALSE, TRUE, TRUE, FALSE, 0, 0, NONE},

	{{&ball, &ball, &ball, &ball, &ball, &ball},
		0, FALSE, FALSE, TRUE, FALSE, TYPE_BALL, SIZE_BALL, NONE},

	{{&ball_up, &ball_up, &ball_up, &ball_up, &ball_up, &ball_up},
		9, FALSE, TRUE, TRUE, FALSE, 0, 0, NONE},

	{{&land, &land, &land, &land, &land, &land},
		5, FALSE, FALSE, TRUE, TRUE, TYPE_STAND, SIZE_STAND, ACT_STAND},

	{{&flip_change, &flip_change, &flip_change, &flip_change, &flip_change, &flip_change},
		5, FALSE, FALSE, TRUE, TRUE, TYPE_STAND, SIZE_STAND, ACT_STAND},

	{{&fall, &jump_gun_out, &jump_gun_up, &jump_gun_down, &jump_gun_diag_up, &jump_gun_diag_down},
		1, TRUE, FALSE, TRUE, FALSE, TYPE_JUMP_DOWN, SIZE_STAND, ACT_LAND},

	{{&jump_up, &jump_gun_out, &jump_gun_up, &jump_gun_down, &jump_gun_diag_up, &jump_gun_diag_down},
		1, TRUE, FALSE, TRUE, FALSE, TYPE_JUMP_UP, SIZE_STAND, ACT_JUMP_DOWN},

	{{&jump_down, &jump_gun_out, &jump_gun_up, &jump_gun_down, &jump_gun_diag_up, &jump_gun_diag_down},
		1, TRUE, FALSE, TRUE, FALSE, TYPE_JUMP_DOWN, SIZE_STAND, ACT_LAND},

	{{&turbo_jump_start, &turbo_jump_start, &turbo_jump_start, &turbo_jump_start, &turbo_jump_start, &turbo_jump_start},
		45, TRUE, FALSE, TRUE, TRUE, TYPE_STAND, SIZE_STAND, ACT_TURBO_JUMP},

	{{&turbo_jump, &turbo_jump, &turbo_jump, &turbo_jump, &turbo_jump, &turbo_jump},
		1, TRUE, FALSE, TRUE, TRUE, TYPE_TURBO, SIZE_STAND, ACT_TURBO_JUMP_END},

	{{&turbo_end, &turbo_end, &turbo_end, &turbo_end, &turbo_end, &turbo_end},
		58, TRUE, FALSE, TRUE, TRUE, 0, SIZE_STAND, ACT_FALL},

	{{&turbo_charge, &turbo_charge, &turbo_charge, &turbo_charge, &turbo_charge, &turbo_charge},
		1, TRUE, FALSE, TRUE, TRUE, TYPE_TURBO, SIZE_DUCK, ACT_TURBO_CHARGE_END},

	{{&turbo_charge, &turbo_charge, &turbo_charge, &turbo_charge, &turbo_charge, &turbo_charge},
		50, TRUE, FALSE, TRUE, TRUE, 0, SIZE_DUCK, ACT_STAND},

	{{&jump_gun_out, &jump_gun_out, &jump_gun_up, &jump_gun_down, &jump_gun_diag_up, &jump_gun_diag_down},
		1, TRUE, FALSE, TRUE, FALSE, TYPE_JUMP_UP | TYPE_BOUNCE, SIZE_STAND, ACT_JUMP_DOWN},

	{{&ball, &ball, &ball, &ball, &ball, &ball},
		1, FALSE, FALSE, TRUE, FALSE, TYPE_BALL | TYPE_JUMP_UP, SIZE_BALL, ACT_BALL_JUMP_DOWN},

	{{&ball, &ball, &ball, &ball, &ball, &ball},
		1, FALSE,	FALSE, TRUE, FALSE, TYPE_BALL | TYPE_JUMP_DOWN, SIZE_BALL, ACT_BALL},

	{{&ball, &ball, &ball, &ball, &ball, &ball},
		1, FALSE,	FALSE, TRUE, FALSE, TYPE_BALL | TYPE_JUMP_UP | TYPE_BOUNCE, SIZE_BALL, ACT_BALL_JUMP_DOWN},
		
	//rubber chage
	{{&ball, &ball, &ball, &ball, &ball, &ball},
		1, FALSE,	FALSE, TRUE, TRUE, TYPE_BALL, SIZE_BALL, ACT_TURBO_BALL_BOUNCE},
		
	//turbo ball bounce
	{{&ball, &ball, &ball, &ball, &ball, &ball},
		1, FALSE,	FALSE, TRUE, TRUE, TYPE_BALL | TYPE_TURBO, SIZE_BALL, ACT_BALL_JUMP_DOWN},

	{{&flip_change, &flip_change, &flip_change, &flip_change, &flip_change, &flip_change},
		5, FALSE, FALSE, TRUE, TRUE, TYPE_JUMP_UP, SIZE_STAND, ACT_FLIP_JUMP_UP},

	{{&flip_jump, &flip_jump, &flip_jump, &flip_jump, &flip_jump, &flip_jump},
		1, FALSE, FALSE, TRUE, FALSE, TYPE_JUMP_UP | TYPE_FLIP, SIZE_DUCK, ACT_FLIP_JUMP_DOWN},

	{{&flip_jump, &flip_jump, &flip_jump, &flip_jump, &flip_jump, &flip_jump},
		1, FALSE, FALSE, TRUE, FALSE, TYPE_JUMP_DOWN | TYPE_FLIP, SIZE_DUCK, ACT_FLIP_LAND},

	{{&flip_change, &flip_change, &flip_change, &flip_change, &flip_change, &flip_change},
		5, FALSE, FALSE, TRUE, TRUE, TYPE_JUMP_UP, SIZE_STAND, ACT_SPACE_JUMP_UP},

	{{&space_jump, &space_jump, &space_jump, &space_jump, &space_jump, &space_jump},
		1, FALSE, FALSE, TRUE, FALSE, TYPE_JUMP_UP | TYPE_FLIP | TYPE_SPACE, SIZE_DUCK, ACT_SPACE_JUMP_DOWN},

	{{&space_jump, &space_jump, &space_jump, &space_jump, &space_jump, &space_jump},
		1, FALSE, FALSE, TRUE, FALSE, TYPE_JUMP_DOWN | TYPE_FLIP | TYPE_SPACE, SIZE_DUCK, ACT_FLIP_LAND},

	{{&wall_kick_ready, &wall_kick_ready, &wall_kick_ready, &wall_kick_ready, &wall_kick_ready, &wall_kick_ready},
		50, FALSE, TRUE, TRUE, FALSE, 0, 0, NONE},

	{{&wall_kick, &wall_kick, &wall_kick, &wall_kick, &wall_kick, &wall_kick},
		8, FALSE, FALSE, TRUE, FALSE, TYPE_JUMP_UP | TYPE_FLIP, SIZE_STAND, ACT_FLIP_JUMP_UP},

	{{&wall_kick, &wall_kick, &wall_kick, &wall_kick, &wall_kick, &wall_kick},
		8, FALSE, FALSE, TRUE, FALSE, TYPE_JUMP_UP | TYPE_FLIP, SIZE_STAND, ACT_SPACE_JUMP_UP}

};
