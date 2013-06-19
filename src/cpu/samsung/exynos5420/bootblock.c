/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "clk.h"
#include "wakeup.h"
#include "cpu.h"

void bootblock_cpu_init(void);
void bootblock_cpu_init(void)
{
	u32 ret;
	/*
	 * During Suspend-Resume & S/W-Reset, as soon as PMU releases
	 * pad retention, CKE goes high. This causes memory contents
	 * not to be retained during DRAM initialization. Therfore,
	 * there is a new control register(0x100431e8[28]) which lets us
	 * release pad retention and retain the memory content until the
	 * initialization is complete.
	 */
	if (read32(((void *)INF_REG_BASE + INF_REG1_OFFSET)) == S5P_CHECK_SLEEP) {
		write32(PAD_RETENTION_DRAM_COREBLK_VAL,
			(void *)PAD_RETENTION_DRAM_COREBLK_OPTION);
		do {
			ret = read32((void *)PAD_RETENTION_DRAM_STATUS);
		} while (ret != 0x1);
	}

	/* kick off the multi-core timer.
	 * We want to do this as early as we can.
	 */
	mct_start();

	if (get_wakeup_state() == WAKEUP_DIRECT) {
		wakeup();
		/* Never returns. */
	}

	/* For most ARM systems, we have to initialize firmware media source
	 * (ex, SPI, SD/MMC, or eMMC) now; but for Exynos platform, that is
	 * already handled by iROM so there's no need to setup again.
	 */
}
