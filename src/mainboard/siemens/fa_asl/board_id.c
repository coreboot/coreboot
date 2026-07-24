/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <types.h>
#include "board_id.h"

/* Get Board ID via EC I/O port write/read */
int get_board_id(void)
{
	static int id = 0;

	return (id & BOARD_ID_MASK);
}
