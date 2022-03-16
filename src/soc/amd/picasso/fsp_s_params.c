/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <amdblocks/ioapic.h>
#include <device/pci.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/platform_descriptors.h>
#include <soc/soc_util.h>
#include <fsp/api.h>
#include "chip.h"

static void fsps_update_emmc_config(FSP_S_CONFIG *scfg,
				const struct soc_amd_picasso_config *cfg)
{
	int val = SD_DISABLE;

	switch (cfg->emmc_config.timing) {
	case SD_EMMC_DISABLE:
		val = SD_DISABLE;
		break;
	case SD_EMMC_SD_LOW_SPEED:
		val = SD_LOW_SPEED;
		break;
	case SD_EMMC_SD_HIGH_SPEED:
		val = SD_HIGH_SPEED;
		break;
	case SD_EMMC_SD_UHS_I_SDR_50:
		val = SD_UHS_I_SDR_50;
		break;
	case SD_EMMC_SD_UHS_I_DDR_50:
		val = SD_UHS_I_DDR_50;
		break;
	case SD_EMMC_SD_UHS_I_SDR_104:
		val = SD_UHS_I_SDR_104;
		break;
	case SD_EMMC_EMMC_SDR_26:
		val = EMMC_SDR_26;
		break;
	case SD_EMMC_EMMC_SDR_52:
		val = EMMC_SDR_52;
		break;
	case SD_EMMC_EMMC_DDR_104:
		val = EMMC_DDR_104;
		break;
	case SD_EMMC_EMMC_HS200:
		val = EMMC_HS200;
		break;
	case SD_EMMC_EMMC_HS400:
		val = EMMC_HS400;
		break;
	case SD_EMMC_EMMC_HS300:
		val = EMMC_HS300;
		break;
	default:
		break;
	}

	scfg->emmc0_mode = val;
	scfg->emmc0_sdr104_hs400_driver_strength =
		cfg->emmc_config.sdr104_hs400_driver_strength;
	scfg->emmc0_ddr50_driver_strength = cfg->emmc_config.ddr50_driver_strength;
	scfg->emmc0_sdr50_driver_strength = cfg->emmc_config.sdr50_driver_strength;
	scfg->emmc0_init_khz_preset = cfg->emmc_config.init_khz_preset;
}

static void fill_dxio_descriptors(FSP_S_CONFIG *scfg,
			const fsp_dxio_descriptor *descs, size_t num)
{
	size_t i;

	ASSERT_MSG(num <= FSPS_UPD_DXIO_DESCRIPTOR_COUNT,
			"Too many DXIO descriptors provided.");

	for (i = 0; i < num; i++) {
		memcpy(scfg->dxio_descriptor[i], &descs[i], sizeof(scfg->dxio_descriptor[0]));
	}
}

static void fill_ddi_descriptors(FSP_S_CONFIG *scfg,
			const fsp_ddi_descriptor *descs, size_t num)
{
	size_t i;

	ASSERT_MSG(num <= FSPS_UPD_DDI_DESCRIPTOR_COUNT,
			"Too many DDI descriptors provided.");

	for (i = 0; i < num; i++) {
		memcpy(&scfg->ddi_descriptor[i], &descs[i], sizeof(scfg->ddi_descriptor[0]));
	}
}

static void fsp_fill_pcie_ddi_descriptors(FSP_S_CONFIG *scfg)
{
	const fsp_dxio_descriptor *fsp_dxio = NULL;
	const fsp_ddi_descriptor *fsp_ddi = NULL;
	size_t num_dxio = 0;
	size_t num_ddi = 0;

	mainboard_get_dxio_ddi_descriptors(&fsp_dxio, &num_dxio,
						&fsp_ddi, &num_ddi);
	fill_dxio_descriptors(scfg, fsp_dxio, num_dxio);
	fill_ddi_descriptors(scfg, fsp_ddi, num_ddi);
}

static void fsp_usb_oem_customization(FSP_S_CONFIG *scfg,
			const struct soc_amd_picasso_config *cfg)
{
	size_t i;

	ASSERT(FSPS_UPD_USB2_PORT_COUNT == USB_PORT_COUNT);
	/* each OC mapping in xhci_oc_pin_select is 4 bit per USB port */
	ASSERT(2 * sizeof(scfg->xhci_oc_pin_select) >= USB_PORT_COUNT);

	scfg->fch_usb_3_port_force_gen1 = cfg->usb3_port_force_gen1.usb3_port_force_gen1_en;

	if (cfg->has_usb2_phy_tune_params) {
		for (i = 0; i < FSPS_UPD_USB2_PORT_COUNT; i++) {
			memcpy(scfg->fch_usb_2_port_phy_tune[i],
				&cfg->usb_2_port_tune_params[i],
				sizeof(scfg->fch_usb_2_port_phy_tune[0]));
		}
	}

	/* lowest nibble of xhci_oc_pin_select corresponds to OC mapping of first USB port */
	for (i = 0; i < USB_PORT_COUNT; i++) {
		scfg->xhci_oc_pin_select &= ~(0xf << (i * 4));
		scfg->xhci_oc_pin_select |=
			(cfg->usb_port_overcurrent_pin[i] & 0xf) << (i * 4);
	}

	if ((get_silicon_type() == SILICON_RV2) && cfg->usb3_phy_override) {
		scfg->usb_3_phy_enable = cfg->usb3_phy_override;
		for (i = 0; i < FSPS_UPD_RV2_USB3_PORT_COUNT; i++) {
			memcpy(scfg->usb_3_port_phy_tune[i],
				&cfg->usb3_phy_tune_params[i],
				sizeof(scfg->usb_3_port_phy_tune[0]));
		}
		scfg->usb_3_rx_vref_ctrl = cfg->usb3_rx_vref_ctrl;
		scfg->usb_3_rx_vref_ctrl_en = cfg->usb3_rx_vref_ctrl_en;
		scfg->usb_3_tx_vboost_lvl = cfg->usb_3_tx_vboost_lvl;
		scfg->usb_3_tx_vboost_lvl_en = cfg->usb_3_tx_vboost_lvl_en;
		scfg->usb_3_rx_vref_ctrl_x = cfg->usb_3_rx_vref_ctrl_x;
		scfg->usb_3_rx_vref_ctrl_en_x = cfg->usb_3_rx_vref_ctrl_en_x;
		scfg->usb_3_tx_vboost_lvl_x = cfg->usb_3_tx_vboost_lvl_x;
		scfg->usb_3_tx_vboost_lvl_en_x = cfg->usb_3_tx_vboost_lvl_en_x;
	}
}

static void fsp_assign_ioapic_upds(FSP_S_CONFIG *scfg)
{
	scfg->gnb_ioapic_base = GNB_IO_APIC_ADDR;
	scfg->gnb_ioapic_id = GNB_IOAPIC_ID;
	scfg->fch_ioapic_id = FCH_IOAPIC_ID;
}

static void fsp_edp_tuning_upds(FSP_S_CONFIG *scfg,
			const struct soc_amd_picasso_config *cfg)
{
	if (cfg->edp_phy_override & ENABLE_EDP_TUNINGSET) {
		scfg->edp_phy_override = cfg->edp_phy_override;
		scfg->edp_physel = cfg->edp_physel;
		scfg->edp_dp_vs_pemph_level = cfg->edp_tuningset.dp_vs_pemph_level;
		scfg->edp_margin_deemph = cfg->edp_tuningset.margin_deemph;
		scfg->edp_deemph_6db_4 = cfg->edp_tuningset.deemph_6db4;
		scfg->edp_boost_adj = cfg->edp_tuningset.boostadj;
	}
	if (cfg->edp_pwr_adjust_enable) {
		scfg->pwron_digon_to_de = cfg->pwron_digon_to_de;
		scfg->pwron_de_to_varybl = cfg->pwron_de_to_varybl;
		scfg->pwrdown_varybloff_to_de = cfg->pwrdown_varybloff_to_de;
		scfg->pwrdown_de_to_digoff = cfg->pwrdown_de_to_digoff;
		scfg->pwroff_delay = cfg->pwroff_delay;
		scfg->pwron_varybl_to_blon = cfg->pwron_varybl_to_blon;
		scfg->pwrdown_bloff_to_varybloff = cfg->pwrdown_bloff_to_varybloff;
		scfg->min_allowed_bl_level = cfg->min_allowed_bl_level;
	}
}

static void fsp_assign_vbios_upds(FSP_S_CONFIG *scfg)
{
	scfg->vbios_buffer_addr = CONFIG(RUN_FSP_GOP) ? PCI_VGA_RAM_IMAGE_START : 0;
}

void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	const struct soc_amd_picasso_config *cfg;
	FSP_S_CONFIG *scfg = &supd->FspsConfig;

	cfg = config_of_soc();
	fsps_update_emmc_config(scfg, cfg);
	fsp_fill_pcie_ddi_descriptors(scfg);
	fsp_assign_ioapic_upds(scfg);
	fsp_usb_oem_customization(scfg, cfg);
	fsp_edp_tuning_upds(scfg, cfg);
	fsp_assign_vbios_upds(scfg);
}
