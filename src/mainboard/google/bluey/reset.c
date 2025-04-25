/* SPDX-License-Identifier: GPL-2.0-only */

#include <ec/google/chromeec/ec.h>
#include <reset.h>

/* Can't do a "real" reset before the PMIC is initialized in QcLib (romstage),
   but this works well enough for our purposes. */
void do_board_reset(void)
{
	google_chromeec_reboot(EC_REBOOT_COLD, 0);
}
