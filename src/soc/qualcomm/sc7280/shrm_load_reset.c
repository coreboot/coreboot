/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <arch/mmu.h>
#include <console/console.h>
#include <program_loading.h>
#include <soc/mmu.h>
#include <soc/mmu_common.h>
#include <soc/shrm.h>
#include <soc/clock.h>
#include <soc/symbols_common.h>

void shrm_fw_load_reset(void)
{
	struct prog shrm_fw_prog =
		PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/shrm");

	/* map to cached region to force address to be 4 byte aligned */
	mmu_config_range((void *)_shrm, REGION_SIZE(shrm), CACHED_RAM);

	if (!selfload(&shrm_fw_prog))
		die("SOC image: SHRM load failed");

	/* flush cached region */
	dcache_clean_by_mva(_shrm, REGION_SIZE(shrm));
	/* remap back to device memory */
	mmu_config_range((void *)_shrm, REGION_SIZE(shrm), DEV_MEM);

	clock_reset_shrm();

	printk(BIOS_DEBUG, "SOC:SHRM brought out of reset.\n");
}
