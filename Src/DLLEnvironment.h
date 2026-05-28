// C Header File
// Created 1/31/2003; 2:03:28 PM

#ifndef __DLL_ENVIRONMENT__
#define __DLL_ENVIRONMENT__

#include "environment.h"
#include "player.h"

void environment_setup(char type, char water_type, short start, short end, short speed);
void environment_process();
void environment_draw(unsigned char *water_gfx);

#endif
