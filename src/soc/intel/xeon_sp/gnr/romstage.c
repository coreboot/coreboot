/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/romstage.h>

static uint8_t get_mmcfg_base_upd_index(const uint64_t base_addr)
{
	switch (base_addr) {
	case 1ULL * GiB:		// 1G
		return 0;
	case 1ULL * GiB + 512ULL * MiB:	// 1.5G
		return 0x1;
	case 1ULL * GiB + 768ULL * MiB:	// 1.75G
		return 0x2;
	case 2ULL * GiB:		// 2G
		return 0x3;
	case 2ULL * GiB + 256ULL * MiB:	// 2.25G
		return 0x4;
	case 3ULL * GiB:		// 3G
		return 0x5;
	default:			// Auto
		return 0x6;
	}
}

static uint8_t get_mmcfg_size_upd_index(const uint64_t size)
{
	switch (size) {
	case 64ULL * MiB:	// 64M
		return 0;
	case 128ULL * MiB:	// 128M
		return 0x1;
	case 256ULL * MiB:	// 256M
		return 0x2;
	case 512ULL * MiB:	// 512M
		return 0x3;
	case 1ULL * GiB:	// 1G
		return 0x4;
	case 2ULL * GiB:	// 2G
		return 0x5;
	default:		// Auto
		return 0x6;
	}
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;
	m_cfg->mmCfgBase = get_mmcfg_base_upd_index(CONFIG_ECAM_MMCONF_BASE_ADDRESS);
	m_cfg->mmCfgSize = get_mmcfg_size_upd_index(CONFIG_ECAM_MMCONF_LENGTH);

	/* Board level settings */
	mainboard_memory_init_params(mupd);
}
