/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <amdblocks/gpio.h>
#include <soc/pci_devs.h>
#include <platform_descriptors.h>

#define WLAN_DEVFN	PCIE_GPP_2_0_DEVFN
#define SD_DEVFN	PCIE_GPP_2_1_DEVFN
#define NVME_DEVFN	PCIE_GPP_2_2_DEVFN

/* This function provides base GPIO configuration table. */
void baseboard_gpio_table(const struct soc_amd_gpio **gpio, size_t *size);

/*
 * This function allows variant to override any GPIOs that are different than the base GPIO
 * configuration provided by baseboard_gpio_table().
 */
void variant_override_gpio_table(const struct soc_amd_gpio **gpio, size_t *size);

/* This function provides GPIO init in bootblock. */
void variant_bootblock_gpio_table(const struct soc_amd_gpio **gpio, size_t *size);

/* This function provides early GPIO init in early bootblock or psp. */
void variant_early_gpio_table(const struct soc_amd_gpio **gpio, size_t *size);

/* This function allows variant to override any early GPIO init in early bootblock or psp. */
void variant_early_override_gpio_table(const struct soc_amd_gpio **gpio, size_t *size);

/* This function provides GPIO settings before entering sleep. */
void variant_sleep_gpio_table(const struct soc_amd_gpio **gpio, size_t *size);

/* This function provides GPIO settings for eSPI bus. */
void variant_espi_gpio_table(const struct soc_amd_gpio **gpio, size_t *size);

/* This function provides GPIO settings for TPM i2c bus. */
void variant_tpm_gpio_table(const struct soc_amd_gpio **gpio, size_t *size);

/* This function provides GPIO settings before PCIe enumeration. */
void variant_pcie_gpio_table(const struct soc_amd_gpio **gpio, size_t *size);

#endif /* __BASEBOARD_VARIANTS_H__ */
