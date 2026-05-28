// C Source File
// Created 7/15/02; 7:32:04 PM

#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files

#include "utility.h"
#include "fflgui.h"
#include "inventory.h"

const char *slot_get_name(INVENTORY_SLOT *slot)
{
	register short type = slot->type;
	register short index = slot->index;
	
	if(type == TYPE_EQUIPABLE) return "";
	else if(type == TYPE_ABILITY) return "";
	else if(type == TYPE_MODULE) return "";
	
	return "";
}