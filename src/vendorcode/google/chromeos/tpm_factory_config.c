/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <security/tpm/tss.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>

#define CHROMEBOOK_PLUS_HARD_BRANDED BIT(4)
#define CHROMEBOOK_PLUS_SOFT_BRANDED BIT(0)
#define CHROMEBOOK_PLUS_DEVICE (CHROMEBOOK_PLUS_HARD_BRANDED | CHROMEBOOK_PLUS_SOFT_BRANDED)

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
 * Determines whether a ChromeOS device is branded as a Chromebook Plus
 * based on specific bit flags:
 *
 * - Bit 4 (0x10): Indicates whether the device chassis has the
 *                 "chromebook-plus" branding.
 * - Bits 3-0 (0x1): Must be 0x1 to signify compliance with Chromebook Plus
 *                   hardware specifications.
 *
 * To be considered a Chromebook Plus, either of these conditions needs to be met.
 */
bool chromeos_device_branded_plus(void)
{
	uint64_t factory_config = chromeos_get_factory_config();

	if (factory_config == UNDEFINED_FACTORY_CONFIG)
		return false;

	return factory_config & CHROMEBOOK_PLUS_DEVICE;
}
