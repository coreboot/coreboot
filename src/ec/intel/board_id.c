/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <boardid.h>
#include "board_id.h"
#include <ec/acpi/ec.h>
#include <ec/google/chromeec/ec.h>
#include <types.h>

static uint32_t get_board_id_via_ext_ec(void)
{
	uint32_t id = BOARD_ID_INIT;

	if (google_chromeec_get_board_version(&id))
		id = BOARD_ID_UNKNOWN;

	return id;
}

/* Get Board ID via EC I/O port write/read */
int get_rvp_board_id(void)
{
	MAYBE_STATIC_NONZERO int id = BOARD_ID_UNKNOWN;

	if (CONFIG(EC_GOOGLE_CHROMEEC)) { /* CHROME_EC */
		id = get_board_id_via_ext_ec();
	} else { /* WINDOWS_EC */
		if (send_ec_command(EC_FAB_ID_CMD) == 0) {
			id = recv_ec_data() << 8;
			id |= recv_ec_data();
		}
	}
	return (id & BOARD_ID_MASK);
}
