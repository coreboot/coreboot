/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <stddef.h>
#include <boardid.h>
#include <ec/google/chromeec/ec.h>
#include <FspmUpd.h>
#include <soc/amd/picasso/chip.h>
#include <soc/platform_descriptors.h>

const struct soc_amd_gpio *variant_early_gpio_table(size_t *size);

/* This function provides base GPIO configuration table. */
const struct soc_amd_gpio *baseboard_gpio_table(size_t *size);
/*
 * This function allows variant to override any GPIOs that are different than the base GPIO
 * configuration provided by baseboard_gpio_table().
 */
const struct soc_amd_gpio *variant_override_gpio_table(size_t *size);

/* This function provides GPIO init in bootblock. */
const struct soc_amd_gpio *variant_bootblock_gpio_table(size_t *size, int slp_typ);

/*
 * This function provides GPIO table for the pads that need to be configured when entering
 * sleep.
 */
const struct soc_amd_gpio *variant_sleep_gpio_table(size_t *size, int slp_typ);

/* This function provides GPIO settings for eSPI bus. */
const struct soc_amd_gpio *variant_espi_gpio_table(size_t *size);

/* This function provides GPIO settings for TPM i2c bus. */
const struct soc_amd_gpio *variant_tpm_gpio_table(size_t *size);

void variant_updm_update(FSP_M_CONFIG *mcfg);

/* Program any required GPIOs at the finalize phase */
void finalize_gpios(int slp_typ);
/* Modify devictree settings during ramstage. */
void variant_devtree_update(void);
/* Update audio configuration in devicetree during ramstage. */
void variant_audio_update(void);
/* Update bluetooth configuration in devicetree during ramstage. */
void variant_bluetooth_update(void);
/* Update touchscreen configuration in devicetree during ramstage. */
void variant_touchscreen_update(void);
/* Configure PCIe GPIOs as per baseboard sequencing requirements. */
void baseboard_pcie_gpio_configure(void);

/* Per variant FSP-S initialization, default implementation in baseboard and
 * overridable by the variant. */
void variant_get_dxio_ddi_descriptors(const fsp_dxio_descriptor **dxio_descs,
				      size_t *dxio_num,
				      const fsp_ddi_descriptor **ddi_descs,
				      size_t *ddi_num);

/* Provide the descriptors for the associated baseboard for the variant. These functions
 * can be used for obtaining the baseboard's descriptors if the variant followed the
 * baseboard. */
const fsp_dxio_descriptor *baseboard_get_dxio_descriptors(size_t *num);
const fsp_ddi_descriptor *baseboard_get_ddi_descriptors(size_t *num);

/* Retrieve attributes from FW_CONFIG in CBI. */
/* Return value of SAR config. */
int variant_gets_sar_config(void);
/* Return value of Mainboard Type config */
int variant_gets_mb_type_config(void);
/* Return 0 if non-existent, 1 if present. */
int variant_has_emmc(void);
/* Return 0 if non-existent, 1 if present. */
int variant_has_nvme(void);
/* Return 0 if non-existent, 1 if present. */
int variant_has_wwan(void);
/* Add variant is_convertible to identify convertible sku */
int variant_is_convertible(void);

/* Determine if booting in factory by using CROS_SKU_UNPROVISIONED. */
int boot_is_factory_unprovisioned(void);

/* Return true if variant uses v3 version of reference schematics. */
bool variant_uses_v3_schematics(void);
/* Return true if variant uses v3.6 version of reference schematics. */
bool variant_uses_v3_6_schematics(void);
/* Return true if variant uses CODEC_GPI pin for headphone jack interrupt. */
bool variant_uses_codec_gpi(void);
/* Return true if variant has active low power enable for WiFi. */
bool variant_has_active_low_wifi_power(void);
/* Return value of daughterboard ID */
int variant_get_daughterboard_id(void);
/* Return true if the board has a fingerprint sensor. */
bool variant_has_fingerprint(void);
/* Return true if the board needs an extra fpmcu delay. */
bool fpmcu_needs_delay(void);

#endif /* __BASEBOARD_VARIANTS_H__ */
