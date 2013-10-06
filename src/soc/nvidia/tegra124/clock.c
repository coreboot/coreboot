/*
 * Copyright (c) 2013, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <delay.h>
#include <arch/io.h>
#include <soc/addressmap.h>

#include "clk_rst.h"
#include "clock.h"

static struct clk_rst_ctlr *clk_rst = (void *)TEGRA_CLK_RST_BASE;
/*
 * On poweron, AVP clock source (also called system clock) is set to PLLP_out0
 * with frequency set at 1MHz. Before initializing PLLP, we need to move the
 * system clock's source to CLK_M temporarily. And then switch it to PLLP_out4
 * (204MHz) at a later time.
 */
void set_avp_clock_to_clkm(void)
{
	u32 val;

	val = (SCLK_SOURCE_CLKM << SCLK_SWAKEUP_FIQ_SOURCE_SHIFT) |
		(SCLK_SOURCE_CLKM << SCLK_SWAKEUP_IRQ_SOURCE_SHIFT) |
		(SCLK_SOURCE_CLKM << SCLK_SWAKEUP_RUN_SOURCE_SHIFT) |
		(SCLK_SOURCE_CLKM << SCLK_SWAKEUP_IDLE_SOURCE_SHIFT) |
		(SCLK_SYS_STATE_RUN << SCLK_SYS_STATE_SHIFT);
	writel(val, &clk_rst->crc_sclk_brst_pol);
	/* Wait 2-3us for the clock to flush thru the logic as per the TRM */
	udelay(3);
}
