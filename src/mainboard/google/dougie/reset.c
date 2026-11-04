/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <ec/google/chromeec/ec.h>
#include <reset.h>
#include <soc/addressmap.h>

void do_board_reset(void)
{
	if (CONFIG(EC_GOOGLE_CHROMEEC))
		google_chromeec_reboot(EC_REBOOT_COLD, EC_REBOOT_FLAG_IMMEDIATE);
	else
		write32((void *)AOSS_PS_HOLD, 0);
}
