/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include "commands.h"

uint32_t board_id(void)
{
	MAYBE_STATIC_NONZERO uint32_t id = BOARD_ID_INIT;

	if (id == BOARD_ID_INIT) {
		uint8_t ec_id;
		if (wilco_ec_get_board_id(&ec_id) <= 0)
			id = BOARD_ID_UNKNOWN;
		else
			id = ec_id;
	}

	return id;
}
