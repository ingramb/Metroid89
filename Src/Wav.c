// C Source File
// Created 7/1/02; 8:28:38 PM

#define USE_TI89              // Produce .89z File

#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

// #define SAVE_SCREEN        // Save/Restore LCD Contents

#include <tigcclib.h>         // Include All Header Files

extern void _playwav();

// Main Function
void _main(void)
{
	_playwav();
}
