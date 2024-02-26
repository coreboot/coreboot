/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <drivers/vpd/vpd.h>
#include <security/tpm/tss.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>

#define CHROMEBOOK_PLUS_HARD_BRANDED BIT(4)

uint64_t chromeos_get_factory_config(void)
{
	static uint64_t factory_config = UNDEFINED_FACTORY_CONFIG;

	if (factory_config != UNDEFINED_FACTORY_CONFIG)
		return factory_config;

	/* Initialize TPM driver. */
	tpm_result_t rc = tlcl_lib_init();
	if (rc != TPM_SUCCESS) {
		printk(BIOS_ERR, "%s:%d - tlcl_lib_init() failed: %#x\n",
		       __func__, __LINE__, rc);
		return UNDEFINED_FACTORY_CONFIG;
	}

	rc = tlcl_cr50_get_factory_config(&factory_config);

	if (rc != TPM_SUCCESS) {
		printk(BIOS_ERR, "%s:%d - tlcl_cr50_get_factory_config() failed: %#x\n",
		       __func__, __LINE__, rc);
		return UNDEFINED_FACTORY_CONFIG;
	}

	assert(factory_config != UNDEFINED_FACTORY_CONFIG);

	return factory_config;
}

/*
 * Determines whether a ChromeOS device is branded as a Chromebook-Plus
 * based on specific bit flags:
 *
 * - Bit 4 (0x10): Indicates whether the device chassis has the
 *                 "chromebook-plus" branding.
 */
bool chromeos_device_branded_plus_hard(void)
{
	uint64_t factory_config = chromeos_get_factory_config();

	if (factory_config == UNDEFINED_FACTORY_CONFIG)
		return false;

	return (factory_config & CHROMEBOOK_PLUS_HARD_BRANDED) == CHROMEBOOK_PLUS_HARD_BRANDED;
}

/*
 * Use 'feature_level' populated by ChromeOS libsegmentation library to know if the device
 * is a chromebook plus or not.
 *
 * Note: After powerwash or dev/normal mode switch, the splash screen may be incorrect
 * on first boot until VPD is updated.
 */
bool chromeos_device_branded_plus_soft(void)
{
	if (vpd_get_feature_level() > 1)
		return true;

	return false;
}
