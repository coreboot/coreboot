/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>

void variant_update_fspm_upds(FSPM_UPD *memupd)
{
	if (fw_config_probe(FW_CONFIG(DB_LTE, LTE_USB))) {
		FSP_M_CONFIG *m_cfg = &memupd->FspmConfig;
		printk(BIOS_INFO, "Disabling PCIe RP 6 UPD for USB WWAN\n");
		m_cfg->PcieRpEnableMask &= ~BIT(5);
	}
}
