// C Source File
// Created 3/13/2003; 6:00:34 PM

#define __IN_DLL__
#include <tigcclib.h>
#include "shots.h"
#include "utility.h"
#include "animation.h"

const SHOT_COMBO combo_data[SHOT_END_OF_LIST] = {
	{STYPE_NORMAL, COMBO_NORMAL, 1, FALSE}, //normal
	{STYPE_WAVE, COMBO_WAVE, 1, TRUE}, //normal wave
	{STYPE_ICE, COMBO_NORMAL, 1, FALSE}, //normal ice
	{STYPE_ICE, COMBO_WAVE, 1, TRUE}, //normal ice wave

	{STYPE_SPAZER, COMBO_SPREAD, 2, FALSE}, //spazer
	{STYPE_SPAZER, COMBO_TRISPLIT, 2, TRUE}, //spazer wave
	{STYPE_SPAZER_ICE, COMBO_SPREAD, 2, FALSE}, //spazer ice
	{STYPE_SPAZER_ICE, COMBO_TRISPLIT, 2, TRUE}, //spazer ice wave

	{STYPE_PLASMA, COMBO_NORMAL, 10, FALSE}, //plasma
	{STYPE_PLASMA, COMBO_SPLIT, 5, TRUE}, //plasma wave
	{STYPE_PLASMA_ICE, COMBO_NORMAL, 10, FALSE}, //plasma ice
	{STYPE_PLASMA_ICE, COMBO_SPLIT, 5, TRUE}, //plasma ice wave

	{STYPE_CHARGE, COMBO_NORMAL, 6, FALSE}, //normal charge
	{STYPE_CHARGE, COMBO_SPLIT, 3, TRUE}, //normal charge wave
	{STYPE_CHARGE_ICE, COMBO_NORMAL, 6, FALSE}, //normal ice charge
	{STYPE_CHARGE_ICE, COMBO_SPLIT, 3, TRUE}, //normal ice charge wave

	{STYPE_CHARGE_SPAZER, COMBO_SPREAD, 8, FALSE}, //spazer charge
	{STYPE_CHARGE_SPAZER, COMBO_TRISPLIT, 8, TRUE}, //spazer charge wave
	{STYPE_CHARGE_SPAZER_ICE, COMBO_SPREAD, 8, FALSE}, //spazer ice charge
	{STYPE_CHARGE_SPAZER_ICE, COMBO_TRISPLIT, 8, TRUE}, //spazer ice charge wave

	{STYPE_CHARGE_PLASMA, COMBO_NORMAL, 30, FALSE}, //plasma charge
	{STYPE_CHARGE_PLASMA, COMBO_SPLIT, 15, TRUE}, //plasma charge wave
	{STYPE_CHARGE_PLASMA_ICE, COMBO_NORMAL, 30, FALSE}, //plasma ice charge
	{STYPE_CHARGE_PLASMA_ICE, COMBO_SPLIT, 15, TRUE}, //plasma ice charge wave

	{STYPE_MISSILE, COMBO_NORMAL, 5, FALSE}, //missile
	{STYPE_SUPERMISSILE, COMBO_NORMAL, 20, FALSE}, //super missile
	{STYPE_HOMING_MISSILE, COMBO_SPREAD, 5, FALSE}, //homing missle
	{STYPE_ENERGY_LEACH, COMBO_NORMAL, 0, FALSE}, //energy leach missile

	{STYPE_ENEMY_PLASMA, COMBO_NORMAL, 20, TRUE}, //enemy plasma
	{STYPE_ENEMY_ACID, COMBO_NORMAL, 20, FALSE}, //enemy acid
	{STYPE_ENEMY_SHOCKWAVE, COMBO_NORMAL, 30, TRUE}, //enemy shockwave
	{STYPE_ENEMY_SPIKE, COMBO_NORMAL, 10, TRUE}, //enemy spike
	{STYPE_ENEMY_WAVE, COMBO_NORMAL, 20, TRUE}, //enemy wave
	{STYPE_ENEMY_FIREBALL, COMBO_NORMAL, 100, FALSE}, //enemy fireball
	{STYPE_ENEMY_FIREBLIP, COMBO_NORMAL, 40, FALSE}, //enemy fireblip
};

const SHOT_DATA shot_data[STYPE_END_OF_LIST] = {
	{{0, -1, -1}, 1,
		DAMAGE_NORMAL, 12, 6, 10, NONE, 200, 250, NONE, 0, SMALL_EXPLODE, NONE, FALSE, FALSE}, //normal shot

	{{8, 9, 10}, 0,
		DAMAGE_NORMAL, 8, 8, 8, NONE, 200, 250, SHOCKWAVE_SMALL, 6, SMALL_EXPLODE, NONE, FALSE , FALSE}, //normal wave

	{{11, 12, 13}, 0,
		DAMAGE_NORMAL, 7, 7, 7, NONE, 200, 250, ICE_DUST, 8, SMALL_EXPLODE, NONE, TRUE , FALSE}, //normal ice

	{{14, -1, -1}, 2,
		DAMAGE_NORMAL, 15, 3, 12, 1, 200, 250, NONE, 0, TINY_EXPLODE, NONE, FALSE , FALSE}, //spazer

	{{18, -1, -1}, 2,
		DAMAGE_NORMAL, 15, 3, 12, 1, 200, 250, ICE_DUST, 8, TINY_EXPLODE, NONE, TRUE , FALSE}, //spazer ice

	{{22, -1, -1}, 2,
		DAMAGE_PLASMA, 24, 3, 19, 1, 200, 250, NONE, 0, SMALL_EXPLODE, NONE, FALSE, TRUE}, //plasma

	{{26, -1, -1}, 2,
		DAMAGE_PLASMA, 24, 3, 19, 1, 200, 250, ICE_DUST, 8, SMALL_EXPLODE, NONE, TRUE, TRUE}, //plasma ice

	{{30, 31, 32}, 0,
		DAMAGE_CHARGE, 12, 12, 12, NONE, 200, 250, SPARKLE, 8, LARGE_EXPLODE, NONE, FALSE , FALSE}, //charge normal

	{{33, 34, 35}, 0,
		DAMAGE_CHARGE, 15, 15, 15, NONE, 200, 250, ICE_DUST, 8, LARGE_EXPLODE, NONE, TRUE , FALSE}, //charge ice

	{{36, -1, -1}, 2,
		DAMAGE_CHARGE, 24, 5, 18, 2, 200, 250, SPARKLE, 8, SMALL_EXPLODE, NONE, FALSE , FALSE}, //charge spazer

	{{40, -1, -1}, 2,
		DAMAGE_CHARGE, 24, 5, 18, 2, 200, 250, ICE_DUST, 8, SMALL_EXPLODE, NONE, TRUE , FALSE}, //charge spazer ice

	{{44, -1, -1}, 2,
		DAMAGE_CHARGE_PLASMA, 24, 7, 21, 3, 200, 250, SPARKLE, 8, LARGE_EXPLODE, NONE, FALSE, TRUE}, //charge plasma

	{{48, -1, -1}, 2,
		DAMAGE_CHARGE_PLASMA, 24, 7, 21, 3, 200, 250, ICE_DUST, 8, LARGE_EXPLODE, NONE, TRUE, TRUE}, //charge plasma ice

	{{52, -1, -1}, 1,
		DAMAGE_MISSILE, 13, 8, 12, 2, 100, 400, DUST_CLOUD, 3, LARGE_EXPLODE, NONE, FALSE, FALSE}, //missile

	{{60, -1, -1}, 1,
		DAMAGE_SUPERMISSILE, 14, 8, 12, 2, 100, 500, DUST_CLOUD, 3, HUGE_EXPLODE, NONE, FALSE, FALSE}, //super missile

	{{52, -1, -1}, 1,
		DAMAGE_MISSILE, 13, 8, 12, 2, 100, 150, DUST_CLOUD, 3, LARGE_EXPLODE, HEAT_SEAKING, FALSE, FALSE}, //homing missile

	{{68, -1, -1}, 1,
		DAMAGE_SUPERMISSILE, 16, 8, 13, 2, 100, 500, DUST_CLOUD, 3, HUGE_EXPLODE, LEACH, FALSE, FALSE}, //energy leach missile

	{{76, -1, -1}, 0,
		DAMAGE_NORMAL, 24, 4, 0, NONE, 150, 150, NONE, 0, NONE, NONE, FALSE, FALSE}, //enemy plasma

	{{77, -1, -1}, 3,
		DAMAGE_NORMAL, 11, 9, 0, NONE, 100, 150, NONE, 0, SMALL_EXPLODE, ANGLE_DECAY, FALSE, FALSE}, //enemy acid

	{{79, -1, -1}, 3,
		DAMAGE_NORMAL, 7, 24, 0, NONE, 150, 150, NONE, 0, NONE, NONE, FALSE, FALSE}, //enemy shockwave

	{{81, -1, -1}, 1,
		DAMAGE_NORMAL, 7, 3, 6, 1, 40, 60, NONE, 0, NONE, NONE, FALSE, FALSE}, //enemy spike

	{{8, 9, 10}, 0,
		DAMAGE_NORMAL, 8, 8, 8, NONE, 50, 75, SHOCKWAVE_SMALL, 8, SMALL_EXPLODE, NONE, FALSE , FALSE}, //enemy wave

	{{89, 90, 91}, 0,
		DAMAGE_NORMAL, 13, 13, 13, NONE, 250, 250, NONE, 0, SMALL_EXPLODE, NONE, FALSE, FALSE}, //enemy fireball
		
	{{93, 94, 95}, 0,
		DAMAGE_NORMAL, 8, 8, 8, NONE, 200, 200, NONE, 0, TINY_EXPLODE, NONE, FALSE, FALSE}, //enemy fireblip
};