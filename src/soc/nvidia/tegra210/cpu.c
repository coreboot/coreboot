/*
 * This file is part of the coreboot project.
 *
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

#include <device/mmio.h>
#include <assert.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/clk_rst.h>
#include <soc/cpu.h>
#include <soc/secure_boot.h>

static void enable_core_clocks(int cpu)
{
	const uint32_t cpu_clocks[] = {
		[0] = CRC_RST_CPUG_CLR_CPU0 | CRC_RST_CPUG_CLR_DBG0 |
		CRC_RST_CPUG_CLR_CORE0 | CRC_RST_CPUG_CLR_CX0,
		[1] = CRC_RST_CPUG_CLR_CPU1 | CRC_RST_CPUG_CLR_DBG1 |
		CRC_RST_CPUG_CLR_CORE1 | CRC_RST_CPUG_CLR_CX1,
		[2] = CRC_RST_CPUG_CLR_CPU2 | CRC_RST_CPUG_CLR_DBG2 |
		CRC_RST_CPUG_CLR_CORE2 | CRC_RST_CPUG_CLR_CX2,
		[3] = CRC_RST_CPUG_CLR_CPU3 | CRC_RST_CPUG_CLR_DBG3 |
		CRC_RST_CPUG_CLR_CORE3 | CRC_RST_CPUG_CLR_CX3,
	};

	assert (cpu < CONFIG_MAX_CPUS);

	/* Clear reset of CPU components. */
	write32(CLK_RST_REG(rst_cpug_cmplx_clr), cpu_clocks[cpu]);
}

void cpu_prepare_startup(void *entry_64)
{
	struct tegra_secure_boot *sb =
		(struct tegra_secure_boot *)TEGRA_SB_BASE;

	/*
	 * T210 TRM, section 12.4.4.2: "SB_AA64_RESET_LOW_0[0:0] is used to
	 * decide between CPU boot up in AARCH32 (=0) or AARCH64 (=1) mode.
	 * This bit .. is sampled only during 'cold reset of CPU'. Before the
	 * CPU is powered up, the CPU reset vector is loaded in
	 * EVP_CPU_REST_VECTOR_0 for 32-bit boot mode .... However, the CPU
	 * decides to boot in 32-/64-bit mode based on
	 * SB_AA64_RESET_LOW_0[0:0]. If this bit is set (=1), the CPU boots in
	 * 64-bit mode using SB_AA64_RESET_* as the reset address. If this bit
	 * is clear (=0), CPU boots in 32-bit mode using EVP_CPU_RESET_VECTOR."
	 */

	write32(&sb->sb_aa64_reset_low, (uintptr_t)entry_64);
	setbits32(&sb->sb_aa64_reset_low, 1);
	write32(&sb->sb_aa64_reset_high, 0);
}

void start_cpu_silent(int cpu, void *entry_64)
{
	cpu_prepare_startup(entry_64);
	enable_core_clocks(cpu);
}

void start_cpu(int cpu, void *entry_64)
{
	printk(BIOS_DEBUG, "Starting CPU%d @ %p.\n", cpu, entry_64);
	start_cpu_silent(cpu, entry_64);
}
