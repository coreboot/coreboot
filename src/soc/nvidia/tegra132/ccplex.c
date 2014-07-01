/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <string.h>
#include <console/console.h>
#include <arch/io.h>
#include <cbfs.h>
#include <timer.h>
#include <soc/addressmap.h>
#include "clk_rst.h"
#include "ccplex.h"
#include "mc.h"
#include "pmc.h"

#define CLK_RST_REGS (void *)(uintptr_t)(TEGRA_CLK_RST_BASE)
#define PMC_REGS (void *)(uintptr_t)(TEGRA_PMC_BASE)
#define MTS_FILE_NAME "mts"

static int ccplex_start(void)
{
	struct mono_time t1, t2;
	const long timeout_us = 1500000;
	long wait_time;
	const uint32_t handshake_mask = 1;
	const uint32_t cxreset1_mask = 1 << 21;
	uint32_t reg;
	struct tegra_pmc_regs * const pmc = PMC_REGS;
	struct clk_rst_ctlr * const clk_rst = CLK_RST_REGS;

	/* Set the handshake bit to be knocked down. */
	write32(handshake_mask, &pmc->scratch118);

	/* Assert nCXRSET[1] */
	reg = read32(&clk_rst->rst_cpu_cmplx_set);
	reg |= cxreset1_mask;
	write32(reg, &clk_rst->rst_cpu_cmplx_set);

	timer_monotonic_get(&t1);
	while (1) {
		reg = read32(&pmc->scratch118);
		timer_monotonic_get(&t2);

		wait_time = mono_time_diff_microseconds(&t1, &t2);

		/* Wait for the bit to be knocked down. */
		if ((reg & handshake_mask) != handshake_mask)
			break;

		if (wait_time >= timeout_us) {
			printk(BIOS_DEBUG, "MTS handshake timeout.\n");
			return -1;
		}
	}

	printk(BIOS_DEBUG, "MTS handshake took %ld us.\n", wait_time);

	return 0;
}

int ccplex_load_mts(void)
{
	struct cbfs_file file;
	ssize_t offset;
	size_t nread;
	/*
	 * MTS location is hard coded to this magic address. The hardware will
	 * take the MTS from this location and place it in the final resting
	 * place in the carveout region.
	 */
	void * const mts = (void *)(uintptr_t)MTS_LOAD_ADDRESS;
	struct cbfs_media *media = CBFS_DEFAULT_MEDIA;

	offset = cbfs_locate_file(media, &file, MTS_FILE_NAME);
	if (offset < 0) {
		printk(BIOS_DEBUG, "MTS file not found: %s\n", MTS_FILE_NAME);
		return -1;
	}

	/* Read MTS file into the carveout region. */
	nread = cbfs_read(media, mts, offset, file.len);

	if (nread != file.len) {
		printk(BIOS_DEBUG, "MTS bytes read (%zu) != file length(%u)!\n",
			nread, file.len);
		return -1;
	}

	printk(BIOS_DEBUG, "MTS: %zu bytes loaded @ %p\n", nread, mts);

	return ccplex_start();
}
