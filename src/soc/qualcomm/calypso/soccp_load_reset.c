/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <arch/mmu.h>
#include <cbfs.h>
#include <console/console.h>
#include <program_loading.h>
#include <soc/aop_common.h>
#include <soc/clock.h>
#include <soc/mmu.h>
#include <soc/mmu_common.h>
#include <soc/qclib_common.h>
#include <soc/soccp.h>
#include <soc/symbols_common.h>
#include <symbols.h>

void soccp_fw_load(void)
{
	/* map to cached region to force address to be 4 byte aligned */
	mmu_config_range((void *)_dram_soccp, REGION_SIZE(dram_soccp), CACHED_RAM);

	struct prog soccp_fw_prog =
		PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/soccp");

	if (!selfload(&soccp_fw_prog))
		die("SOC image: SOCCP load failed");

	dcache_clean_by_mva(_dram_soccp, REGION_SIZE(dram_soccp));
	mmu_config_range((void *)_dram_soccp, REGION_SIZE(dram_soccp), DEV_MEM);

	/* map to cached region to force address to be 4 byte aligned */
	mmu_config_range((void *)_dram_soccp_dtb, REGION_SIZE(dram_soccp_dtb), CACHED_RAM);

	struct prog soccp_dtb_fw_prog =
		PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/soccp_dtb");

	if (!selfload(&soccp_dtb_fw_prog))
		die("SOC image: SOCCP DTB load failed");

	dcache_clean_by_mva(_dram_soccp_dtb, REGION_SIZE(dram_soccp_dtb));
	mmu_config_range((void *)_dram_soccp_dtb, REGION_SIZE(dram_soccp_dtb), DEV_MEM);
}
