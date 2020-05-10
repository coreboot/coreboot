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
			const picasso_fsp_pcie_descriptor *descs, size_t num)
{
	size_t i;
	picasso_fsp_pcie_descriptor *fsp_pcie;

	/* FIXME: this violates C rules. */
	fsp_pcie = (picasso_fsp_pcie_descriptor *)(scfg->dxio_descriptor0);

	for (i = 0; i < num; i++) {
		fsp_pcie[i] = descs[i];
	}
}

static void fill_ddi_descriptors(FSP_S_CONFIG *scfg,
			const picasso_fsp_ddi_descriptor *descs, size_t num)
{
	size_t i;
	picasso_fsp_ddi_descriptor *fsp_ddi;

	/* FIXME: this violates C rules. */
	fsp_ddi = (picasso_fsp_ddi_descriptor *)&(scfg->ddi_descriptor0);

	for (i = 0; i < num; i++) {
		fsp_ddi[i] = descs[i];
	}
}
static void fsp_fill_pcie_ddi_descriptors(FSP_S_CONFIG *scfg)
{
	const picasso_fsp_pcie_descriptor *fsp_pcie;
	const picasso_fsp_ddi_descriptor *fsp_ddi;
	size_t num_pcie;
	size_t num_ddi;

	mainboard_get_pcie_ddi_descriptors(&fsp_pcie, &num_pcie,
						&fsp_ddi, &num_ddi);
	fill_pcie_descriptors(scfg, fsp_pcie, num_pcie);
	fill_ddi_descriptors(scfg, fsp_ddi, num_ddi);
}

void platform_fsp_silicon_init_params_cb(FSPS_UPD *supd)
{
	const struct soc_amd_picasso_config *cfg;
	FSP_S_CONFIG *scfg = &supd->FspsConfig;

	cfg = config_of_soc();
	fsps_update_emmc_config(scfg, cfg);
	fsp_fill_pcie_ddi_descriptors(scfg);
}
