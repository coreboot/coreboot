/* tag: legacybios starter
 *
 * Copyright (C) 2003 Stefan Reinauer
 *
 * See the file "COPYING" for further information about
 * the copyright and warranty status of this work.
 */

#include "config.h"
#include "types.h"

#define FIXED_ROMSTART 0xc0000
#define FIXED_ROMEND   0xcffff

void legacybios(ucell romstart, ucell romend);

void cmain(void)
{
	legacybios(FIXED_ROMSTART,FIXED_ROMEND);
	return;
}
