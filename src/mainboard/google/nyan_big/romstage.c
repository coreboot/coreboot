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

#include <arch/cache.h>
#include <arch/cpu.h>
#include <arch/exception.h>
#include <arch/stages.h>
#include <device/device.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include "sdram_configs.h"
#include "soc/nvidia/tegra124/chip.h"
#include "soc/nvidia/tegra124/sdram.h"
#include <soc/display.h>
#include <timestamp.h>

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

static void __attribute__((noinline)) romstage(void)
{
	int dram_size_mb;
#if CONFIG_COLLECT_TIMESTAMPS
	uint64_t romstage_start_time = timestamp_get();
#endif

	configure_l2ctlr();
	configure_l2actlr();

	console_init();
	exception_init();

	sdram_init(get_sdram_config());

	/* used for MMU and CBMEM setup */
	dram_size_mb = sdram_size_mb();

	u32 dram_start = (CONFIG_SYS_SDRAM_BASE >> 20);
	u32 dram_end = dram_start + dram_size_mb;	/* plus one... */

	mmu_init();
	mmu_config_range(0, dram_start, DCACHE_OFF);
	mmu_config_range(dram_start, dram_size_mb, DCACHE_WRITEBACK);
	mmu_config_range(CONFIG_DRAM_DMA_START >> 20,
			 CONFIG_DRAM_DMA_SIZE >> 20, DCACHE_OFF);
	mmu_config_range(dram_end, 4096 - dram_end, DCACHE_OFF);
	mmu_disable_range(0, 1);
	dcache_mmu_enable();

	/* For quality of the user experience, it's important to get
	 * the video going ASAP. Because there are long delays in some
	 * of the powerup steps, we do some very early setup here in
	 * romstage. The only thing setup_display does is manage
	 * 4 GPIOs, under control of the config struct members.
	 * In general, it is safe to enable panel power, and disable
	 * anything related to the backlight. If we get something wrong,
	 * we can easily fix it in ramstage by further GPIO manipulation,
	 * so we feel it is ok to do some setting at this point.
	 */

	const struct device *soc = dev_find_slot(DEVICE_PATH_CPU_CLUSTER, 0);
	printk(BIOS_SPEW, "s%s: soc is %p\n", __func__, soc);
	if (soc && soc->chip_info) {
		const struct soc_nvidia_tegra124_config *config =
			soc->chip_info;
		setup_display((struct soc_nvidia_tegra124_config *)config);
	}

	cbmem_initialize_empty();

#if CONFIG_COLLECT_TIMESTAMPS
	timestamp_init(0);
	timestamp_add(TS_START_ROMSTAGE, romstage_start_time);
	timestamp_add(TS_START_COPYRAM, timestamp_get());
#endif
	void *entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA,
				      "fallback/coreboot_ram");
#if CONFIG_COLLECT_TIMESTAMPS
	timestamp_add(TS_END_COPYRAM, timestamp_get());
#endif
	stage_exit(entry);
}

/* Stub to force arm_init_caches to the top, before any stack/memory accesses */
void main(void)
{
	asm ("bl arm_init_caches" ::: "r0","r1","r2","r3","r4","r5","ip");
	romstage();
}
