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
	write32(&pmc->scratch118, handshake_mask);

	/* Assert nCXRSET[1] */
	reg = read32(CLK_RST_REG(rst_cpu_cmplx_set));
	reg |= cxreset1_mask;
	write32(CLK_RST_REG(rst_cpu_cmplx_set), reg);

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
	ssize_t nread;
	struct stopwatch sw;
	struct cbfsf mts_file;
	struct region_device fh;

	/*
	 * MTS location is hard coded to this magic address. The hardware will
	 * take the MTS from this location and place it in the final resting
	 * place in the carveout region.
	 */
	void * const mts = (void *)(uintptr_t)MTS_LOAD_ADDRESS;

	stopwatch_init(&sw);
	if (cbfs_boot_locate(&mts_file, MTS_FILE_NAME, NULL)) {
		printk(BIOS_DEBUG, "MTS file not found: %s\n", MTS_FILE_NAME);
		return -1;
	}

	cbfs_file_data(&fh, &mts_file);

	/* Read MTS file into the carveout region. */
	nread = rdev_readat(&fh, mts, 0, region_device_sz(&fh));

	if (nread != region_device_sz(&fh)) {
		printk(BIOS_DEBUG, "MTS bytes read (%zu) != file length(%u)!\n",
			nread, region_device_sz(&fh));
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

	stopwatch_init(&sw);

	/* Perform cluster 0 ram repair */
	reg = read32(&flow->ram_repair);
	reg |= req;
	write32(&flow->ram_repair, reg);
	while ((read32(&flow->ram_repair) & sts) != sts)
		;

	/* Perform cluster 1 ram repair */
	reg = read32(&flow->ram_repair_cluster1);
	reg |= req;
	write32(&flow->ram_repair_cluster1, reg);
	while ((read32(&flow->ram_repair_cluster1) & sts) != sts)
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
	write32(CLK_RST_REG(rst_cpug_cmplx_clr), CRC_RST_CPUG_CLR_NONCPU);

	/* Clear reset of L2 and CoreSight components. */
	write32(CLK_RST_REG(rst_cpug_cmplx_clr),
		CRC_RST_CPUG_CLR_L2 | CRC_RST_CPUG_CLR_PDBG);
}

void ccplex_cpu_start(void *entry_addr)
{
	/* Enable common clocks for the shared resources between the cores. */
	start_common_clocks();

	start_cpu(0, entry_addr);
}
