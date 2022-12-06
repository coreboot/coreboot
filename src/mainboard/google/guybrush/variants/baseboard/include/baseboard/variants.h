/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <gpio.h>
#include <soc/pci_devs.h>
#include <platform_descriptors.h>

#define WLAN_DEVFN	PCIE_GPP_2_0_DEVFN
#define SD_DEVFN	PCIE_GPP_2_1_DEVFN
#define WWAN_DEVFN	PCIE_GPP_2_2_DEVFN
#define NVME_DEVFN	PCIE_GPP_2_3_DEVFN

/* This function provides base GPIO configuration table. */
const struct soc_amd_gpio *baseboard_gpio_table(size_t *size);
/*
 * These functions allow variants to override any GPIOs that are different than the base GPIO
 * configuration provided without having to replace the entire file.
 */
const struct soc_amd_gpio *variant_override_gpio_table(size_t *size);
const struct soc_amd_gpio *variant_early_override_gpio_table(size_t *size);
const struct soc_amd_gpio *variant_bootblock_override_gpio_table(size_t *size);
const struct soc_amd_gpio *variant_romstage_override_gpio_table(size_t *size);

/* This function provides early GPIO init in early bootblock or psp. */
const struct soc_amd_gpio *variant_early_gpio_table(size_t *size);

/* This function provides GPIO settings at the end of bootblock. */
const struct soc_amd_gpio *variant_bootblock_gpio_table(size_t *size);

/* This function provides GPIO settings before PCIe enumeration. */
const struct soc_amd_gpio *baseboard_romstage_gpio_table(size_t *size);

/* This function provides GPIO settings for eSPI bus. */
const struct soc_amd_gpio *variant_espi_gpio_table(size_t *size);

/* This function provides GPIO settings for TPM i2c bus. */
const struct soc_amd_gpio *variant_tpm_gpio_table(size_t *size);

bool variant_has_pcie_wwan(void);

void variant_update_dxio_descriptors(fsp_dxio_descriptor *dxio_descriptors);
void variant_update_ddi_descriptors(fsp_ddi_descriptor *ddi_descriptors);

enum dxio_port_id {
	WLAN,
	SD,
	WWAN_NVME,
	NVME
};

uint8_t variant_sd_aux_reset_gpio(void);

void variant_devtree_update(void);
#endif /* __BASEBOARD_VARIANTS_H__ */
