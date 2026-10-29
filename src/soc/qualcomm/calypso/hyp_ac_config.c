/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <arch/mmu.h>
#include <console/console.h>
#include <program_loading.h>
#include <soc/mmu.h>
#include <soc/mmu_common.h>
#include <soc/hyp_ac_config.h>
#include <soc/symbols_common.h>

void hyp_ac_config_fw_load(void)
{

	struct prog hypac_cfg = PROG_INIT(PROG_PAYLOAD,
				CONFIG_CBFS_PREFIX"/hypac_cfg");
	if (!selfload(&hypac_cfg))
		die("hypac_cfg load failed");

	dcache_clean_by_mva(_dram_hyp_ac, REGION_SIZE(dram_hyp_ac));
}
