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

#include <arch/io.h>
#include <cbfs.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/clk_rst.h>
#include <soc/ccplex.h>
#include <soc/cpu.h>
#include <soc/flow.h>
#include <soc/mc.h>
#include <soc/pmc.h>
#include <soc/power.h>
#include <soc/romstage.h>
#include <string.h>
#include <timer.h>

#define PMC_REGS (void *)(uintptr_t)(TEGRA_PMC_BASE)
#define MTS_FILE_NAME "mts"

static int ccplex_start(void)
{
	struct stopwatch sw;
	const long timeout_ms = 1500;
	const uint32_t handshake_mask = 1;
	const uint32_t cxreset1_mask = 1 << 21;
	uint32_t reg;
	struct tegra_pmc_regs * const pmc = PMC_REGS;

	/* Set the handshake bit to be knocked down. */
	write32(handshake_mask, &pmc->scratch118);

	/* Assert nCXRSET[1] */
	reg = read32(CLK_RST_REG(rst_cpu_cmplx_set));
	reg |= cxreset1_mask;
	write32(reg, CLK_RST_REG(rst_cpu_cmplx_set));

	stopwatch_init_msecs_expire(&sw, timeout_ms);
	while (1) {
		reg = read32(&pmc->scratch118);

		/* Wait for the bit to be knocked down. */
		if ((reg & handshake_mask) != handshake_mask)
			break;

		if (stopwatch_expired(&sw)) {
			printk(BIOS_DEBUG, "MTS handshake timeout.\n");
			return -1;
		}
	}

	printk(BIOS_DEBUG, "MTS handshake took %ld usecs.\n",
		stopwatch_duration_usecs(&sw));

	return 0;
}

int ccplex_load_mts(void)
{
	struct cbfs_file file;
	ssize_t offset;
	size_t nread;
	struct stopwatch sw;

	/*
	 * MTS location is hard coded to this magic address. The hardware will
	 * take the MTS from this location and place it in the final resting
	 * place in the carveout region.
	 */
	void * const mts = (void *)(uintptr_t)MTS_LOAD_ADDRESS;
	struct cbfs_media *media = CBFS_DEFAULT_MEDIA;

	stopwatch_init(&sw);
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

	printk(BIOS_DEBUG, "MTS: %zu bytes loaded @ %p in %ld usecs.\n",
	       nread, mts, stopwatch_duration_usecs(&sw));

	return ccplex_start();
}

static void enable_cpu_clocks(void)
{
	clock_enable(CLK_ENB_CPU, 0, 0, SET_CLK_ENB_CPUG_ENABLE |
		     SET_CLK_ENB_CPULP_ENABLE, 0, 0);
}

static void enable_cpu_power_partitions(void)
{
	/* Bring up fast cluster, non-CPU, CPU0, and CPU1 partitions. */
	power_ungate_partition(POWER_PARTID_CRAIL);
	power_ungate_partition(POWER_PARTID_C0NC);
	power_ungate_partition(POWER_PARTID_CE0);
	power_ungate_partition(POWER_PARTID_CE1);
}

static void request_ram_repair(void)
{
	struct flow_ctlr * const flow = (void *)(uintptr_t)TEGRA_FLOW_BASE;
	const uint32_t req = 1 << 0;
	const uint32_t sts = 1 << 1;
	uint32_t reg;
	struct stopwatch sw;

	printk(BIOS_DEBUG, "Requesting RAM repair.\n");

	reg = read32(&flow->ram_repair);
	reg |= req;
	write32(reg, &flow->ram_repair);

	stopwatch_init(&sw);
	while ((read32(&flow->ram_repair) & sts) != sts)
		;

	printk(BIOS_DEBUG, "RAM repair complete in %ld usecs.\n",
		stopwatch_duration_usecs(&sw));
}

void ccplex_cpu_prepare(void)
{
	enable_cpu_clocks();
	enable_cpu_power_partitions();

	mainboard_configure_pmc();
	mainboard_enable_vdd_cpu();

	request_ram_repair();
}

static void start_common_clocks(void)
{
	/* Clear fast CPU partition reset. */
	write32(CRC_RST_CPUG_CLR_NONCPU, CLK_RST_REG(rst_cpug_cmplx_clr));

	/* Clear reset of L2 and CoreSight components. */
	write32(CRC_RST_CPUG_CLR_L2 | CRC_RST_CPUG_CLR_PDBG,
		CLK_RST_REG(rst_cpug_cmplx_clr));
}

void ccplex_cpu_start(void *entry_addr)
{
	/* Enable common clocks for the shared resources between the cores. */
	start_common_clocks();

	start_cpu(0, entry_addr);
}
