/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <security/tpm/tss.h>
#include <vendorcode/google/chromeos/chromeos.h>

int64_t chromeos_get_factory_config(void)
{
	static int64_t factory_config = -1;

	if (factory_config >= 0)
		return factory_config;

	/* Initialize TPM driver. */
	tpm_result_t rc = tlcl_lib_init();
	if (rc != TPM_SUCCESS) {
		printk(BIOS_ERR, "%s:%d - tlcl_lib_init() failed: %#x\n",
		       __func__, __LINE__, rc);
		return -1;
	}

	rc = tlcl_cr50_get_factory_config((uint64_t *)&factory_config);

	if (rc != TPM_SUCCESS) {
		printk(BIOS_ERR, "%s:%d - tlcl_cr50_get_factory_config() failed: %#x\n",
		       __func__, __LINE__, rc);
		return -1;
	}

	return factory_config;
}
