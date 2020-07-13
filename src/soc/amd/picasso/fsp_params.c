/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci.h>
#include <soc/pci_devs.h>
#include <soc/platform_descriptors.h>
#include <fsp/api.h>
#include "chip.h"

static void fsps_update_emmc_config(FSP_S_CONFIG *scfg,
				const struct soc_amd_picasso_config *cfg)
{
	int val = SD_DISABLE;

	switch (cfg->sd_emmc_config) {
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
	case SD_EMMC_EMMC_DDR_52:
		val = EMMC_DDR_52;
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
}

static void fill_pcie_descriptors(FSP_S_CONFIG *scfg,
			const fsp_pcie_descriptor *descs, size_t num)
{
	size_t i;

	for (i = 0; i < num; i++) {
		memcpy(scfg->dxio_descriptor[i], &descs[i], sizeof(scfg->dxio_descriptor[0]));
	}
}

static void fill_ddi_descriptors(FSP_S_CONFIG *scfg,
			const fsp_ddi_descriptor *descs, size_t num)
{
	size_t i;

	for (i = 0; i < num; i++) {
		memcpy(&scfg->ddi_descriptor[i], &descs[i], sizeof(scfg->ddi_descriptor[0]));
	}
}
static void fsp_fill_pcie_ddi_descriptors(FSP_S_CONFIG *scfg)
{
	const fsp_pcie_descriptor *fsp_pcie;
	const fsp_ddi_descriptor *fsp_ddi;
	size_t num_pcie;
	size_t num_ddi;

	mainboard_get_pcie_ddi_descriptors(&fsp_pcie, &num_pcie,
						&fsp_ddi, &num_ddi);
	fill_pcie_descriptors(scfg, fsp_pcie, num_pcie);
	fill_ddi_descriptors(scfg, fsp_ddi, num_ddi);
}

static void fsp_usb_oem_customization(FSP_S_CONFIG *scfg,
			const struct soc_amd_picasso_config *cfg)
{
	size_t num = sizeof(struct usb2_phy_tune);

	scfg->xhci0_force_gen1 = cfg->xhci0_force_gen1;

	memcpy(scfg->fch_usb_2_port0_phy_tune, &cfg->usb_2_port_0_tune_params, num);
	memcpy(scfg->fch_usb_2_port1_phy_tune, &cfg->usb_2_port_1_tune_params, num);
	memcpy(scfg->fch_usb_2_port2_phy_tune, &cfg->usb_2_port_2_tune_params, num);
	memcpy(scfg->fch_usb_2_port3_phy_tune, &cfg->usb_2_port_3_tune_params, num);
	memcpy(scfg->fch_usb_2_port4_phy_tune, &cfg->usb_2_port_4_tune_params, num);
	memcpy(scfg->fch_usb_2_port5_phy_tune, &cfg->usb_2_port_5_tune_params, num);
}

void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	const struct soc_amd_picasso_config *cfg;
	FSP_S_CONFIG *scfg = &supd->FspsConfig;

	cfg = config_of_soc();
	fsps_update_emmc_config(scfg, cfg);
	fsp_fill_pcie_ddi_descriptors(scfg);
	fsp_usb_oem_customization(scfg, cfg);
}
