/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <device/pci.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/platform_descriptors.h>
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
	const fsp_dxio_descriptor *fsp_dxio;
	const fsp_ddi_descriptor *fsp_ddi;
	size_t num_dxio;
	size_t num_ddi;

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
}

static void fsp_assign_ioapic_upds(FSP_S_CONFIG *scfg)
{
	_Static_assert(CONFIG_PICASSO_GNB_IOAPIC_ID >= CONFIG_MAX_CPUS,
			"PICASSO_GNB_IOAPIC_ID should be >= CONFIG_MAX_CPUS!\n");
	_Static_assert(CONFIG_PICASSO_FCH_IOAPIC_ID >= CONFIG_MAX_CPUS,
			"PICASSO_FCH_IOAPIC_ID should be >= CONFIG_MAX_CPUS!\n");
	_Static_assert(CONFIG_PICASSO_GNB_IOAPIC_ID != CONFIG_PICASSO_FCH_IOAPIC_ID,
			"PICASSO_GNB_IOAPIC_ID should be != PICASSO_FCH_IOAPIC_ID!\n");

	scfg->gnb_ioapic_base = GNB_IO_APIC_ADDR;
	scfg->gnb_ioapic_id = CONFIG_PICASSO_GNB_IOAPIC_ID;
	scfg->fch_ioapic_id = CONFIG_PICASSO_FCH_IOAPIC_ID;
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
}
