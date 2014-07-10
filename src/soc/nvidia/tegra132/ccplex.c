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
#include <soc/romstage.h>
#include "clk_rst.h"
#include "ccplex.h"
#include "flow.h"
#include "mc.h"
#include "pmc.h"
#include "power.h"

#define EVP_CPU_RESET_VECTOR (void *)(uintptr_t)(TEGRA_EVP_BASE + 0x100)
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

static void enable_cpu_clocks(void)
{
	struct clk_rst_ctlr * const clk_rst = CLK_RST_REGS;
	uint32_t reg;

	reg = read32(&clk_rst->clk_enb_l_set);
	reg |= CLK_ENB_CPU;
	write32(reg, &clk_rst->clk_enb_l_set);

	reg = read32(&clk_rst->clk_enb_v_set);
	reg |= SET_CLK_ENB_CPUG_ENABLE | SET_CLK_ENB_CPULP_ENABLE;
	write32(reg, &clk_rst->clk_enb_v_set);
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
	struct mono_time t1, t2;

	printk(BIOS_DEBUG, "Requesting RAM repair.\n");

	reg = read32(&flow->ram_repair);
	reg |= req;
	write32(reg, &flow->ram_repair);

	timer_monotonic_get(&t1);
	while ((read32(&flow->ram_repair) & sts) != sts);
	timer_monotonic_get(&t2);

	printk(BIOS_DEBUG, "RAM repair complete in %ld usecs.\n",
		mono_time_diff_microseconds(&t1, &t2));
}

void ccplex_cpu_prepare(void)
{
	enable_cpu_clocks();
	enable_cpu_power_partitions();

	mainboard_configure_pmc();
	mainboard_enable_vdd_cpu();

	request_ram_repair();
}

static void start_cpu0(void)
{
	struct clk_rst_ctlr * const clk_rst = CLK_RST_REGS;

	/* Clear fast CPU partition reset. */
	write32(CRC_RST_CPUG_CLR_NONCPU, &clk_rst->rst_cpug_cmplx_clr);

	/* Clear reset of CPU0 components. */
	write32(CRC_RST_CPUG_CLR_CPU0 |
		CRC_RST_CPUG_CLR_DBG0 |
		CRC_RST_CPUG_CLR_CORE0 |
		CRC_RST_CPUG_CLR_CX0 |
		CRC_RST_CPUG_CLR_L2 |
		CRC_RST_CPUG_CLR_PDBG, &clk_rst->rst_cpug_cmplx_clr);
}

/*
 * The Denver cores come up in aarch32 mode. In order to transition to
 * 64-bit mode a write to the RMR (reset mangement register) with the
 * AA64 bit (0) set while setting RR (reset request bit 1).
 */
static const uint32_t aarch32to64[] = {
	0xe3a00003,       /* mov     r0, #3 */
	0xee0c0f50,       /* mcr     15, 0, r0, cr12, cr0, {2} */
};

static void load_aarch64_trampoline(void *addr)
{
	const size_t trampoline_size = sizeof(aarch32to64);
	const void * const trampoline = &aarch32to64[0];

	/* Copy trampoline into ram. */
	memcpy(addr, trampoline, trampoline_size);
}

void ccplex_cpu_start(void *entry_addr)
{
	struct tegra_pmc_regs * const pmc = PMC_REGS;
	void * const evp_cpu_reset_vector = EVP_CPU_RESET_VECTOR;
	void *trampoline;
	uint32_t entry_point;

	/*
	 * Just place the trampoline at the MTS_LOAD_ADDRESS. This assumes
	 * the program to run doesn't overlap this address.
	 */
	const uint32_t trampoline_addr = MTS_LOAD_ADDRESS;
	trampoline = (void *)(uintptr_t)trampoline_addr;

	/* The arm entry points have bit 0 set if thumb code. Mask that off. */
	entry_point = (uint32_t)(uintptr_t)entry_addr;

	load_aarch64_trampoline(trampoline);

	/* Warm reset vector is pulled from the PMC scratch registers. */
	write32(entry_point, &pmc->secure_scratch34);
	write32(0, &pmc->secure_scratch35);

	printk(BIOS_DEBUG, "Starting CPU0 @ %p trampolining to %08x.\n",
		trampoline, entry_point);

	/*
	 * The Denver cores start in 32-bit mode. Therefore a trampoline
	 * is needed to get into 64-bit mode. Point the cold reset vector
	 * to the trampoline location.
	 */
	write32(trampoline_addr, evp_cpu_reset_vector);

	start_cpu0();
}
