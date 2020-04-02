/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <baseboard/variants.h>
#include <boardid.h>
#include <ec/google/chromeec/ec.h>

int variant_board_id(void)
{
	MAYBE_STATIC_NONZERO uint32_t id = BOARD_ID_INIT;

	if (CONFIG(EC_GOOGLE_CHROMEEC)) {
		if (id == BOARD_ID_INIT) {
			if (google_chromeec_get_board_version(&id))
				id = BOARD_ID_UNKNOWN;
		}
		return id;
	} else {
		return 0;
	}
}
