/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <soc/config.h>
#include <smbios.h>
#include <spd.h>
#include <soc/romstage.h>
#include <soc/soc_util.h>

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

	/* fast boot setting */
	int fast_boot_mode = get_fast_boot_mode();
	m_cfg->AttemptFastBoot = !!(fast_boot_mode & XEONSP_FAST_BOOT_WARM);
	m_cfg->AttemptFastBootCold = !!(fast_boot_mode & XEONSP_FAST_BOOT_COLD);

	FSPM_ARCH2_UPD *arch_upd = &mupd->FspmArchUpd;
	if (fast_boot_mode == XEONSP_FAST_BOOT_DISABLED) {
		arch_upd->BootMode =
			FSP_BOOT_WITH_FULL_CONFIGURATION;
		printk(BIOS_NOTICE, "Reset BootMode as "
				"FSP_BOOT_WITH_FULL_CONFIGURATION.\n");
	}

	if (CONFIG(ENABLE_VMX))
		m_cfg->VmxEnable = 1;
	else
		m_cfg->VmxEnable = 0;

	/* Board level settings */
	mainboard_memory_init_params(mupd);
}

uint8_t get_error_correction_type(const uint8_t RasModesEnabled)
{
	//TODO: to update later
	return MEMORY_ARRAY_ECC_UNKNOWN;
}

uint32_t get_max_capacity_mib(void)
{
	//TODO: to update later.
	return 4 * MiB * CONFIG_MAX_SOCKET;
}

uint8_t get_max_dimm_count(void)
{
	return MAX_DIMM;
}

uint8_t get_dram_type(const struct SystemMemoryMapHob *hob)
{
	if (hob->DramType == SPD_MEMORY_TYPE_DDR5_SDRAM)
		return MEMORY_TYPE_DDR5;

	return MEMORY_TYPE_DDR4;
}
