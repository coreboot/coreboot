/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/clk.h>
#include <soc/wakeup.h>

void bootblock_soc_init(void)
{
	if (get_wakeup_state() == WAKEUP_DIRECT) {
		wakeup();
		/* Never returns. */
	}

	/* For most ARM systems, we have to initialize firmware media source
	 * (ex, SPI, SD/MMC, or eMMC) now; but for Exynos platform, that is
	 * already handled by iROM so there's no need to setup again.
	 */
}
