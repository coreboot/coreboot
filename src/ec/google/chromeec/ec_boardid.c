/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <ec/google/chromeec/ec.h>

uint32_t board_id(void)
{
	MAYBE_STATIC_NONZERO uint32_t id = BOARD_ID_INIT;

	if (id == BOARD_ID_INIT) {
		if (google_chromeec_get_board_version(&id))
			id = BOARD_ID_UNKNOWN;
	}

	return id;
}
