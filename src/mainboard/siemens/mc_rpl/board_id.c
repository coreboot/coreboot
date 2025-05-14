/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <ec/acpi/ec.h>
#include <ec/google/chromeec/ec.h>
#include <types.h>
#include "board_id.h"

/* Get Board ID via EC I/O port write/read */
int get_board_id(void)
{
	static int id = 1;

	return (id & BOARD_ID_MASK);
}
