/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <symbols.h>
#include <soc/soc_services.h>
#include "mmu.h"

#define WIFI_IMEM_0_START	((uintptr_t)_wifi_imem_0 / KiB)
#define WIFI_IMEM_0_END		((uintptr_t)_ewifi_imem_0 / KiB)
#define WIFI_IMEM_1_START	((uintptr_t)_wifi_imem_1 / KiB)
#define WIFI_IMEM_1_END		((uintptr_t)_ewifi_imem_1 / KiB)

#define OC_IMEM_START		((uintptr_t)_oc_imem / KiB)
#define OC_IMEM_END		((uintptr_t)_eoc_imem / KiB)

#define DRAM_START		((uintptr_t)_dram / MiB)
#define DRAM_SIZE		(CONFIG_DRAM_SIZE_MB)
#define DRAM_END		(DRAM_START + DRAM_SIZE)

/* DMA memory for drivers */
#define DMA_START            ((uintptr_t)_dma_coherent / MiB)
#define DMA_SIZE             (REGION_SIZE(dma_coherent) / MiB)

void setup_dram_mappings(enum dram_state dram)
{
	if (dram == DRAM_INITIALIZED) {
		mmu_config_range(DRAM_START, DRAM_SIZE, DCACHE_WRITEBACK);
		/* Map DMA memory */
		mmu_config_range(DMA_START, DMA_SIZE, DCACHE_OFF);
		/* Mark cbmem backing store as ready. */
		if (ENV_CREATES_CBMEM)
			ipq_cbmem_backing_store_ready();
	} else {
		mmu_disable_range(DRAM_START, DRAM_SIZE);
		/* Map DMA memory */
		mmu_disable_range(DMA_START, DMA_SIZE);
	}
}

void setup_mmu(enum dram_state dram)
{
	dcache_mmu_disable();

	mmu_init();

	/* start with mapping everything as strongly ordered. */
	mmu_config_range(0, 4096, DCACHE_OFF);

	/* Map Device memory. */
	mmu_config_range_kb(WIFI_IMEM_0_START,
				WIFI_IMEM_0_END - WIFI_IMEM_0_START,
				DCACHE_WRITEBACK);

	mmu_config_range_kb(WIFI_IMEM_1_START,
				WIFI_IMEM_1_END - WIFI_IMEM_1_START,
				DCACHE_WRITEBACK);

	mmu_config_range_kb(OC_IMEM_START,
				OC_IMEM_END - OC_IMEM_START,
				DCACHE_WRITEBACK);

	/* Map DRAM memory */
	setup_dram_mappings(dram);

	mmu_disable_range(DRAM_END, 4096 - DRAM_END);

	/* disable Page 0 for trapping NULL pointer references. */
	mmu_disable_range_kb(0, 1);

	dcache_mmu_enable();
}
