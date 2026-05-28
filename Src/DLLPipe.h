// Header File
// Created 1/13/2005; 2:13:20 PM

#ifndef __DLL_PIPE__
#define __DLL_PIPE__

#include "pipe.h"

char pipe_init(PIPE_HEADER *header, short number);
void pipe_reset();
void pipe_cleanup();
void pipe_open(PIPE *p, short damage_type);
void pipe_process();
void pipe_draw();

#endif

