// C Header File
// Created 1/22/2003; 3:31:36 PM

#ifndef __DLL_ANIMATION__
#define __DLL_ANIMATION__

#include "animation.h"

char animation_setup();
void animation_reset();
void animation_cleanup();
short animation_create(short x, short y, short type, short link);
void animation_delete(short a);
void animation_process();
void animation_draw(SPRITE_HEADER *anim_header, unsigned char *anim_gfx, ENEMY *e);

#endif

