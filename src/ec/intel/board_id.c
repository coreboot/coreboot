/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <boardid.h>
#include "board_id.h"
#include <console/console.h>
#include <ec/acpi/ec.h>
#include <ec/google/chromeec/ec.h>
#include <types.h>

static int intel_ec_get_board_version(uint32_t *id)
{
	if (send_ec_command(EC_FAB_ID_CMD))
		return -1;

	*id = recv_ec_data() << 8 | recv_ec_data();
	return 0;
}

/* Read Board ID from EC */
int get_rvp_board_id(void)
{
	static uint32_t id = BOARD_ID_INIT;
	const char *ec_type;
	int ret;

	/* If already initialized, return the cached board ID. */
	if (id != BOARD_ID_INIT)
		return id;

	/* Reading board ID. */
	if (CONFIG(EC_GOOGLE_CHROMEEC)) { /* Chrome EC */
		ec_type = "ChromeEC";
		ret = google_chromeec_get_board_version(&id);
	} else { /* Intel EC */
		ec_type = "IntelEC";
		ret = intel_ec_get_board_version(&id);
	}

	if (ret == -1) {
		id = BOARD_ID_UNKNOWN;
		printk(BIOS_INFO, "[%s] board id: unknown\n", ec_type);
	} else {
		id &= BOARD_ID_MASK;
		printk(BIOS_INFO, "[%s] board id: 0x%x\n", ec_type, id);
	}

	return id;
}
