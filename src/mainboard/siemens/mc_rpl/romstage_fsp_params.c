/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fsp/api.h>
#include <hwilib.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include <soc/meminit.h>
#include <baseboard/variants.h>

/*
 * ADL-P silicon can support 7 SRC CLK's and 10 CLKREQ signals. Out of 7 SRCCLK's
 * 3 will be used for CPU, the rest are for PCH. If more than 4 PCH devices are
 * connected on the platform, an external differential buffer chip needs to be placed at
 * the platform level.
 *
 * GEN3_EXTERNAL_CLOCK_BUFFER Kconfig is selected for ADL-P RVP (not applicable for
 * ADL-M/N RVP)
 *
 * CONFIG_CLKSRC_FOR_EXTERNAL_BUFFER provides the CLKSRC that feed clock to discrete
 * buffer for further distribution to platform.
 */
static void configure_external_clksrc(FSP_M_CONFIG *m_cfg)
{
	for (unsigned int i = CONFIG_MAX_PCIE_CLOCK_SRC; i < CONFIG_MAX_PCIE_CLOCK_REQ; i++)
		m_cfg->PcieClkSrcUsage[i] = CONFIG_CLKSRC_FOR_EXTERNAL_BUFFER;
}

__weak void rpl_memory_params(FSPM_UPD *memupd)
{
}

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	FSP_M_CONFIG *m_cfg = &memupd->FspmConfig;
	const struct mb_cfg *mem_config = variant_memory_params();

	static struct mem_spd spd_info;
	static uint8_t spd_data[CONFIG_DIMM_SPD_SIZE];
	const char *cbfs_hwi_name = "hwinfo.hex";

	/* Initialize SPD information for LPDDR5 from HW-Info. */
	memset(spd_data, 0, sizeof(spd_data));
	if ((hwilib_find_blocks(cbfs_hwi_name) == CB_SUCCESS) &&
	    (hwilib_get_field(SPD, spd_data, 0x80) == 0x80)) {
		spd_info.topo = MEM_TOPO_MEMORY_DOWN;
		spd_info.spd_data.in_mem = true;
		spd_info.spd_data.ptr = (uintptr_t)spd_data;
		spd_info.spd_data.len = CONFIG_DIMM_SPD_SIZE;
	} else {
		die("No SPD in HW-Info found!\n");
	}

	/*
	 * Set Raptor Lake specific new upds that Alder Lake doesn't have.
	 * This can be removed when Alder Lake and Raptor Lake FSP headers align.
	 */
	rpl_memory_params(memupd);

	memcfg_init(memupd, mem_config, &spd_info, 0);

	if (CONFIG(GEN3_EXTERNAL_CLOCK_BUFFER))
		configure_external_clksrc(m_cfg);

	variant_configure_fspm(memupd);
}
