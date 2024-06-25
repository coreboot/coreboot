/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <console/console.h>

#include "board_id.h"

uint32_t board_id(void)
{
	printk(BIOS_SPEW, "Board ID: 0x%x\n", BOARD_ID_FROST_CREEK);

	return BOARD_ID_FROST_CREEK;
}
