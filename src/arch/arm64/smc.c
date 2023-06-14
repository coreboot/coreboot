/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/smc.h>
#include <console/console.h>
#include <types.h>

/* Assumes at least a PSCI implementation is present */
uint8_t smccc_supports_arch_soc_id(void)
{
	static uint8_t supported = 0xff;
	uint64_t smc_ret;

	if (supported != 0xff)
		return supported;

	// PSCI_FEATURES mandatory from PSCI 1.0
	smc_ret = smc_call0(PSCI_VERSION);
	if (smc_ret < 0x10000)
		goto fail;

	smc_ret = smc_call1(PSCI_FEATURES, SMCCC_VERSION);
	if (smc_ret == PSCI_NOT_SUPPORTED)
		goto fail;

	// SMCCC_ARCH_FEATURES supported from SMCCC 1.1
	smc_ret = smc_call0(SMCCC_VERSION);
	if (smc_ret < 0x10001)
		goto fail;

	smc_ret = smc_call1(SMCCC_ARCH_FEATURES, SMCCC_ARCH_SOC_ID);
	if (smc_ret != SMC_SUCCESS)
		goto fail;

	supported = 1;
	return supported;

fail:
	supported = 0;
	return supported;
}

enum cb_err smccc_arch_soc_id(uint32_t *jep106code, uint32_t *soc_revision)
{
	uint64_t smc_ret;

	if (jep106code == NULL || soc_revision == NULL)
		return CB_ERR_ARG;

	smc_ret = smc_call1(SMCCC_ARCH_SOC_ID, SMCCC_GET_SOC_VERSION);
	if (smc_ret != SMC_INVALID_PARAMETER)
		*jep106code = smc_ret;
	else
		*jep106code = -1;

	smc_ret = smc_call1(SMCCC_ARCH_SOC_ID, SMCCC_GET_SOC_REVISION);
	if (smc_ret != SMC_INVALID_PARAMETER)
		*soc_revision = smc_ret;
	else
		*soc_revision = -1;

	if (*jep106code == -1 || *soc_revision == -1) {
		printk(BIOS_ERR, "SMCCC_ARCH_SOC_ID failed!\n");
		return CB_ERR;
	} else
		return CB_SUCCESS;
}
