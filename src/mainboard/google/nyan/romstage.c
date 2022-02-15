/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <arch/exception.h>
#include <cbmem.h>
#include <console/console.h>
#include <reset.h>
#include <program_loading.h>
#include <soc/addressmap.h>
#include <soc/cache.h>
#include <soc/clk_rst.h>
#include <soc/clock.h>
#include <soc/display.h>
#include <soc/early_configs.h>
#include <soc/nvidia/tegra/i2c.h>
#include <soc/nvidia/tegra124/chip.h>
#include <soc/power.h>
#include <soc/sdram.h>
#include <symbols.h>
#include <timestamp.h>

#include "sdram_configs.h"

static void __attribute__((noinline)) romstage(void)
{
	timestamp_init(0);
	timestamp_add_now(TS_ROMSTAGE_START);

	console_init();
	exception_init();

	sdram_init(get_sdram_config());

	/* used for MMU and CBMEM setup, in MB */
	u32 dram_start_mb = (uintptr_t)_dram/MiB;
	u32 dram_end_mb = sdram_max_addressable_mb();
	u32 dram_size_mb = dram_end_mb - dram_start_mb;

	configure_l2_cache();
	mmu_init();
	/* Device memory below DRAM is uncached. */
	mmu_config_range(0, dram_start_mb, DCACHE_OFF);
	/* SRAM is cached. MMU code will round size up to page size. */
	mmu_config_range((uintptr_t)_sram/MiB,
			 DIV_ROUND_UP(REGION_SIZE(sram), MiB),
			 DCACHE_WRITEBACK);
	/* DRAM is cached. */
	mmu_config_range(dram_start_mb, dram_size_mb, DCACHE_WRITEBACK);
	/* A window for DMA is uncached. */
	mmu_config_range((uintptr_t)_dma_coherent/MiB,
			 REGION_SIZE(dma_coherent)/MiB, DCACHE_OFF);
	/* The space above DRAM is uncached. */
	if (dram_end_mb < 4096)
		mmu_config_range(dram_end_mb, 4096 - dram_end_mb, DCACHE_OFF);
	mmu_disable_range(0, 1);
	dcache_mmu_enable();

	/*
	 * A watchdog reset only resets part of the system so it ends up in
	 * a funny state. If that happens, we need to reset the whole machine.
	 */
	if (power_reset_status() == POWER_RESET_WATCHDOG) {
		printk(BIOS_INFO, "Watchdog reset detected, rebooting.\n");
		board_reset();
	}

	/* FIXME: this may require coordination with moving timestamps */
	cbmem_initialize_empty();

	/* This was already called from verstage in vboot context. */
	if (!CONFIG(VBOOT))
		early_mainboard_init();

	run_ramstage();
}

/* Stub to force arm_init_caches to the top, before any stack/memory accesses */
void main(void)
{
	asm volatile ("bl arm_init_caches"
		      ::: "r0","r1","r2","r3","r4","r5","ip");
	romstage();
}
