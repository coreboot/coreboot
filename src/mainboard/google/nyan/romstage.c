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

#include <arch/cache.h>
#include <arch/cpu.h>
#include <arch/exception.h>
#include <arch/stages.h>
#include <device/device.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include "soc/nvidia/tegra124/chip.h"
#include <soc/display.h>

// Convenient shorthand (in MB)
#define DRAM_START	(CONFIG_SYS_SDRAM_BASE >> 20)
#define DRAM_SIZE	CONFIG_DRAM_SIZE_MB
#define DRAM_END	(DRAM_START + DRAM_SIZE)	/* plus one... */

enum {
	L2CTLR_ECC_PARITY = 0x1 << 21,
	L2CTLR_TAG_RAM_LATENCY_MASK = 0x7 << 6,
	L2CTLR_TAG_RAM_LATENCY_CYCLES_3 = 2 << 6,
	L2CTLR_DATA_RAM_LATENCY_MASK = 0x7 << 0,
	L2CTLR_DATA_RAM_LATENCY_CYCLES_3  = 2 << 0
};

enum {
	L2ACTLR_FORCE_L2_LOGIC_CLOCK_ENABLE_ACTIVE = 0x1 << 27,
	L2ACTLR_ENABLE_HAZARD_DETECT_TIMEOUT = 0x1 << 7,
	L2ACTLR_DISABLE_CLEAN_EVICT_PUSH_EXTERNAL = 0x1 << 3
};

/* Configures L2 Control Register to use 3 cycles for DATA/TAG RAM latency. */
static void configure_l2ctlr(void)
{
   uint32_t val;

   val = read_l2ctlr();
   val &= ~(L2CTLR_DATA_RAM_LATENCY_MASK | L2CTLR_TAG_RAM_LATENCY_MASK);
   val |= (L2CTLR_DATA_RAM_LATENCY_CYCLES_3 | L2CTLR_TAG_RAM_LATENCY_CYCLES_3 |
	   L2CTLR_ECC_PARITY);
   write_l2ctlr(val);
}

/* Configures L2 Auxiliary Control Register for Cortex A15. */
static void configure_l2actlr(void)
{
   uint32_t val;

   val = read_l2actlr();
   val |= (L2ACTLR_DISABLE_CLEAN_EVICT_PUSH_EXTERNAL |
	   L2ACTLR_ENABLE_HAZARD_DETECT_TIMEOUT |
	   L2ACTLR_FORCE_L2_LOGIC_CLOCK_ENABLE_ACTIVE);
   write_l2actlr(val);
}

void main(void)
{
	// Globally disable MMU, caches and branch prediction (these should
	// already be disabled by default on reset).
	uint32_t sctlr = read_sctlr();
	sctlr &= ~(SCTLR_M | SCTLR_C | SCTLR_Z | SCTLR_I);
	write_sctlr(sctlr);

	arm_invalidate_caches();

	// Renable icache and branch prediction.
	sctlr = read_sctlr();
	sctlr |= SCTLR_Z | SCTLR_I;
	write_sctlr(sctlr);

	configure_l2ctlr();
	configure_l2actlr();

	mmu_init();
	mmu_config_range(0, DRAM_START, DCACHE_OFF);
	mmu_config_range(DRAM_START, DRAM_SIZE, DCACHE_WRITEBACK);
	mmu_config_range(CONFIG_DRAM_DMA_START >> 20,
			 CONFIG_DRAM_DMA_SIZE >> 20, DCACHE_OFF);
	mmu_config_range(DRAM_END, 4096 - DRAM_END, DCACHE_OFF);
	mmu_disable_range(0, 1);
	dcache_invalidate_all();
	dcache_mmu_enable();

	exception_init();

	/* for quality of the user interface, it's important to get
	 * the video going ASAP. Because there are long delays in some
	 * of the powerup steps, we do some very early setup here in
	 * romstage. We don't do this in the bootblock because video
	 * setup is finicky and subject to change; hence, we do it as
	 * early as we can in the RW stage, but never in the RO stage.
	 */

	const struct device *soc = dev_find_slot(DEVICE_PATH_CPU_CLUSTER, 0);
	printk(BIOS_SPEW, "s%s: soc is %p\n", __func__, soc);
	if (soc && soc->chip_info) {
		const struct soc_nvidia_tegra124_config *config =
			soc->chip_info;
		setup_display((struct soc_nvidia_tegra124_config *)config);
	}

	cbmem_initialize_empty();

	void *entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA,
				      "fallback/coreboot_ram");
	stage_exit(entry);
}
