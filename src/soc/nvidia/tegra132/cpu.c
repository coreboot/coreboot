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
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/clk_rst.h>
#include <soc/cpu.h>
#include <soc/pmc.h>

#define EVP_CPU_RESET_VECTOR (void *)(uintptr_t)(TEGRA_EVP_BASE + 0x100)
#define PMC_REGS (void *)(uintptr_t)(TEGRA_PMC_BASE)

static void enable_core_clocks(int cpu)
{
	const uint32_t cpu0_clocks = CRC_RST_CPUG_CLR_CPU0 |
					CRC_RST_CPUG_CLR_DBG0 |
					CRC_RST_CPUG_CLR_CORE0 |
					CRC_RST_CPUG_CLR_CX0;
	const uint32_t cpu1_clocks = CRC_RST_CPUG_CLR_CPU1 |
					CRC_RST_CPUG_CLR_DBG1 |
					CRC_RST_CPUG_CLR_CORE1 |
					CRC_RST_CPUG_CLR_CX1;

	/* Clear reset of CPU components. */
	if (cpu == 0)
		write32(CLK_RST_REG(rst_cpug_cmplx_clr), cpu0_clocks);
	else
		write32(CLK_RST_REG(rst_cpug_cmplx_clr), cpu1_clocks);
}

static void set_armv8_32bit_reset_vector(uintptr_t entry)
{
	void * const evp_cpu_reset_vector = EVP_CPU_RESET_VECTOR;
	write32(evp_cpu_reset_vector, entry);
}

static void set_armv8_64bit_reset_vector(uintptr_t entry)
{
	struct tegra_pmc_regs * const pmc = PMC_REGS;

	/* Currently assume 32-bit addresses only. */
	write32(&pmc->secure_scratch34, entry);
	write32(&pmc->secure_scratch35, 0);
}

void cpu_prepare_startup(void *entry_64)
{
	/* Warm reset vector is pulled from the PMC scratch registers. */
	set_armv8_64bit_reset_vector((uintptr_t)entry_64);

	/*
	 * The Denver cores start in 32-bit mode. Therefore a trampoline
	 * is needed to get into 64-bit mode. Point the cold reset vector
	 * to the traompoline location.
	 */
	set_armv8_32bit_reset_vector((uintptr_t)reset_entry_32bit);
}

void start_cpu_silent(int cpu, void *entry_64)
{
	cpu_prepare_startup(entry_64);
	enable_core_clocks(cpu);
}

void start_cpu(int cpu, void *entry_64)
{
	printk(BIOS_DEBUG, "Starting CPU%d @ %p trampolining to %p.\n",
		cpu, reset_entry_32bit, entry_64);

	start_cpu_silent(cpu, entry_64);
}
