/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <fast_spi_def.h>
#include <gpio_defs.h>
#include <hsio.h>
#include <soc/fiamux.h>
#include <string.h>

#ifdef __RAMSTAGE__
static void update_hsio_info_for_m2_slots(size_t num_of_entry, BL_HSIO_INFORMATION *config)
{
	uint32_t reg32;
	bool m2a_pcie, m2b_pcie;
	uint8_t entry;
	BL_FIA_MUX_CONFIG_HOB *fiamux_hob_data = get_fiamux_hob_data();
	uint16_t supported_hsio_lanes;
	void *spibar = fast_spi_get_bar();
	uint32_t hsfs;

	/* Configure FIA MUX PCD */
	supported_hsio_lanes =
		(uint16_t)fiamux_hob_data->FiaMuxConfig.SkuNumLanesAllowed;

	/* Detects modules type */
	// _GPIO_4 : LFFF: M2A_CFGn : M2A_SATAn : 0 SATA, 1 PCIe
	reg32 = read32((void *)PCH_PCR_ADDRESS(PID_GPIO_4, R_PAD_CFG_DW0_GPIO_4));
	m2a_pcie = (reg32 & B_PCH_GPIO_RX_STATE) ? 1 : 0;
	// _GPIO_5 : LFFF: M2A_CFGn : M2A_SATAn : 0 SATA, 1 PCIe
	reg32 = read32((void *)PCH_PCR_ADDRESS(PID_GPIO_5, R_PAD_CFG_DW0_GPIO_5));
	m2b_pcie = (reg32 & B_PCH_GPIO_RX_STATE) ? 1 : 0;

	printk(BIOS_DEBUG,
	       "GPIO values from M2 slots A:%d B:%d "
	       "(0=SATA, 1=PCIe or not populated)\n",
	       m2a_pcie, m2b_pcie);

	// HSIO default config is for PCIe, only update for SATA
	// (also secondary PCIe lines are already set depending on SKU)
	for (entry = 0; entry < num_of_entry; entry++) {
		/* only update the active config */
		if (config[entry].NumLanesSupported != supported_hsio_lanes)
			continue;
		BL_ME_FIA_CONFIG *fia_config = &(config[entry].FiaConfig);
		BL_ME_FIA_MUX_CONFIG *mux_config =
			&(config[entry].FiaConfig.MuxConfiguration);
		BL_ME_FIA_SATA_CONFIG *sata_config =
			&(config[entry].FiaConfig.SataLaneConfiguration);
		if (!m2a_pcie) {
			// change Lane 14 config
			mux_config->BL_MeFiaMuxLaneMuxSel.Lane14MuxSel =
				BL_ME_FIA_MUX_LANE_SATA;
			sata_config->BL_MeFiaSataLaneSataSel.Lane14SataSel =
				BL_ME_FIA_SATA_CONTROLLER_LANE_ASSIGNED;
		}
		if (!m2b_pcie) {
			// change Lane 12 config
			mux_config->BL_MeFiaMuxLaneMuxSel.Lane12MuxSel =
				BL_ME_FIA_MUX_LANE_SATA;
			sata_config->BL_MeFiaSataLaneSataSel.Lane12SataSel =
				BL_ME_FIA_SATA_CONTROLLER_LANE_ASSIGNED;
		}

		/* Check SPIBAR for security override
		   at least one M2 slot is populated with SATA
		   the configuration is different form ME current one */
		hsfs = read32(spibar + SPIBAR_HSFSTS_CTL);
		if ((!(hsfs & SPIBAR_HSFSTS_FDOPSS))
			&& (!m2a_pcie || !m2b_pcie)
			&& memcmp(fia_config,
				  &fiamux_hob_data->FiaMuxConfig.FiaMuxConfig,
				  sizeof(BL_ME_FIA_CONFIG))) {
			/* update configuration to NOT change ME config
			   as it will fail with security override set. */
			memcpy(fia_config,
			       &fiamux_hob_data->FiaMuxConfig.FiaMuxConfig,
			       sizeof(BL_ME_FIA_CONFIG));
			printk(BIOS_CRIT, "FLASH SECURITY OVERRIDE SET: "
					  "M2 SATA Slots are not available!\n");

		}
	}
}
#endif

size_t mainboard_get_hsio_config(BL_HSIO_INFORMATION **p_hsio_config)
{
	size_t num;
	num = ARRAY_SIZE(tagada_hsio_config);
#ifdef __RAMSTAGE__
	update_hsio_info_for_m2_slots(num, tagada_hsio_config);
#endif
	(*p_hsio_config) = (BL_HSIO_INFORMATION *)tagada_hsio_config;
	return num;
}
