/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <device/pci_ops.h>
#include <fsp/api.h>
#include <soc/romstage.h>
#include <soc/soc_chip.h>
#include <string.h>

/*
 * Placeholder to configure GPIO early from romstage relying on the FW_CONFIG.
 *
 * If any platform would like to override early GPIOs, they should override from
 * the variant directory.
 */
__weak void fw_config_configure_pre_mem_gpio(void)
{
	/* Nothing to do */
}

__weak void variant_update_soc_memory_init_params(FSPM_UPD *memupd)
{
	/* Nothing to do */
}

static bool soc_is_pantherlake_h(void)
{
	uint16_t mch_id = pci_s_read_config16(PCI_DEV(0, 0, 0), PCI_DEVICE_ID);

	if (mch_id == 0xffff) {
		printk(BIOS_ERR, "No matching PCI DID present\n");
		return false;
	}

	/*
	 * Identify Panther Lake H by testing all Memory Controller Hub (MCH) IDs utilized on fatcat
	 * devices.
	 */
	switch (mch_id) {
	case PCI_DID_INTEL_PTL_H_ID_1:
	case PCI_DID_INTEL_PTL_H_ID_2:
	case PCI_DID_INTEL_PTL_H_ID_3:
	case PCI_DID_INTEL_PTL_H_ID_4:
		return true;
	default:
		return false;
	}
}

static void disable_vr_settings_on_pantherlake_h(FSP_M_CONFIG *m_cfg)
{
	if (!soc_is_pantherlake_h())
		return;

	/*
	 * The board operates a Panther Lake H SoC; disable the PTL-U VR settings.
	 *
	 * The Voltage Regulator (VR) configurations supplied by the device tree are
	 * specifically adjusted for a Panther Lake U SoC, which is intended for fatcat board
	 * designs. When these settings are applied to a board equipped with a Panther Lake H
	 * SoC, it may experience performance problems under high-stress conditions. This is
	 * because the I_TRIP value is set lower than the device's actual capability.
	 */
	printk(BIOS_INFO, "Disabling VR settings on PTL-H.\n");
	for (size_t i = 0; i < NUM_VR_DOMAINS; i++) {
		m_cfg->CepEnable[i] = false;
		m_cfg->EnableFastVmode[i] = false;
	}
}

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct pad_config *pads;
	size_t pads_num;
	const struct mb_cfg *mem_config = variant_memory_params();
	bool half_populated = variant_is_half_populated();
	struct mem_spd spd_info;

	pads = variant_romstage_gpio_table(&pads_num);
	if (pads_num)
		gpio_configure_pads(pads, pads_num);
	fw_config_configure_pre_mem_gpio();

	memset(&spd_info, 0, sizeof(spd_info));
	variant_get_spd_info(&spd_info);

	memcfg_init(memupd, mem_config, &spd_info, half_populated);

	/* Override FSP-M Voltage Regulator settings on Panther Lake H. */
	disable_vr_settings_on_pantherlake_h(&memupd->FspmConfig);

	/* Override FSP-M UPD per board if required. */
	variant_update_soc_memory_init_params(memupd);
}
