/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmio.h>
#include <console/console.h>
#include <hsio.h>
#include <gpio_defs.h>
#include <soc/fiamux.h>

#ifdef __RAMSTAGE__
static void update_hsio_info_for_m2_slots(size_t num_of_entry, BL_HSIO_INFORMATION *config)
{
	uint32_t reg32;
	bool m2a_pcie, m2b_pcie;
	uint8_t entry;

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
		BL_ME_FIA_MUX_CONFIG *mux_config = &(config[entry].FiaConfig.MuxConfiguration);
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
