/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <arch/mmu.h>
#include <console/console.h>
#include <program_loading.h>
#include <soc/mmu.h>
#include <soc/aop_common.h>
#include <soc/clock.h>
#include <soc/mmu_common.h>
#include <soc/symbols_common.h>
#include <symbols.h>

void aop_fw_load_reset(void)
{
	/* map to cached region to force address to be 4 byte aligned */
	mmu_config_range((void *)_aop_code_ram, REGION_SIZE(aop_code_ram), CACHED_RAM);

	struct prog aop_fw_prog = PROG_INIT(PROG_PAYLOAD, AOP_CBFS_NAME("aop"));

	if (!selfload(&aop_fw_prog))
		die("SOC image: AOP load failed");

	/* flush cached region */
	dcache_clean_by_mva(_aop_code_ram, REGION_SIZE(aop_code_ram));
	/* remap back to device memory */
	mmu_config_range((void *)_aop_code_ram, REGION_SIZE(aop_code_ram), DEV_MEM);

	clock_reset_aop();
}
