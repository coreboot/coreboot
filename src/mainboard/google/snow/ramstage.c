/*
 * Copyright (C) 2013 The ChromeOS Authors
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <console/console.h>
#include <cbmem.h>
#include <arch/exception.h>
#include <cpu/samsung/exynos5250/clk.h>
#include <cpu/samsung/exynos5250/power.h>

void hardwaremain(int boot_complete);
void main(void)
{
	console_init();
	printk(BIOS_INFO, "hello from ramstage; now with deluxe exception handling.\n");

	/* this is going to move, but we must have it now and we're not sure where */
	exception_init();
	/* place at top of physical memory */
	high_tables_size = CONFIG_COREBOOT_TABLES_SIZE;
	high_tables_base = CONFIG_SYS_SDRAM_BASE +
			((CONFIG_DRAM_SIZE_MB << 20UL) * CONFIG_NR_DRAM_BANKS) -
			CONFIG_COREBOOT_TABLES_SIZE;

	const unsigned epll_hz = 192000000;
	const unsigned sample_rate = 48000;
	const unsigned lr_frame_size = 256;
	clock_epll_set_rate(epll_hz);
	clock_select_i2s_clk_source();
	clock_set_i2s_clk_prescaler(epll_hz, sample_rate * lr_frame_size);

	power_enable_xclkout();

	hardwaremain(0);
}
