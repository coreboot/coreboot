/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <console/console.h>

#include "harcuvar_boardid.h"

uint32_t board_id(void)
{
	int id = BoardIdHarcuvar;

	printk(BIOS_SPEW, "Board ID: %#x.\n", id);

	return id;
}
