/* 
 * Copyright (C) 2003 Stefan Reinauer
 *
 * See the file "COPYING" for further information about
 * the copyright and warranty status of this work.
 */

#include "config.h"
#include "types.h"


/*
 * wrap an array around the hex'ed dictionary file
 */

static char romimage[] = {
#include "rom.h"
};

void legacybios(ucell romstart, ucell romend);

void cmain(void)
{
	legacybios((ucell) romimage,
		 (ucell) romimage + sizeof(romimage));
	return;
}
