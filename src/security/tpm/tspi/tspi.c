/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Copyright 2017 Facebook Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/cbmem_console.h>
#include <console/console.h>
#include <reset.h>
#include <security/tpm/tspi.h>
#include <security/tpm/tss.h>
#include <stdlib.h>
#include <string.h>

#if IS_ENABLED(CONFIG_TPM1)
static uint32_t tpm1_invoke_state_machine(void)
{
	uint8_t disable;
	uint8_t deactivated;
	uint32_t result = TPM_SUCCESS;

	/* Check that the TPM is enabled and activated. */
	result = tlcl_get_flags(&disable, &deactivated, NULL);
	if (result != TPM_SUCCESS) {
		printk(BIOS_ERR, "TPM: Can't read capabilities.\n");
		return result;
	}

	if (!!deactivated != IS_ENABLED(CONFIG_TPM_DEACTIVATE)) {
		printk(BIOS_INFO,
		       "TPM: Unexpected TPM deactivated state. Toggling...\n");
		result = tlcl_set_deactivated(!deactivated);
		if (result != TPM_SUCCESS) {
			printk(BIOS_ERR,
			       "TPM: Can't toggle deactivated state.\n");
			return result;
		}

		deactivated = !deactivated;
		result = TPM_E_MUST_REBOOT;
	}

	if (disable && !deactivated) {
		printk(BIOS_INFO, "TPM: disabled (%d). Enabling...\n", disable);

		result = tlcl_set_enable();
		if (result != TPM_SUCCESS) {
			printk(BIOS_ERR, "TPM: Can't set enabled state.\n");
			return result;
		}

		printk(BIOS_INFO, "TPM: Must reboot to re-enable\n");
		result = TPM_E_MUST_REBOOT;
	}

	return result;
}
#endif

/*
 * tpm_setup starts the TPM and establishes the root of trust for the
 * anti-rollback mechanism.  SetupTPM can fail for three reasons.  1 A bug. 2 a
 * TPM hardware failure. 3 An unexpected TPM state due to some attack.  In
 * general we cannot easily distinguish the kind of failure, so our strategy is
 * to reboot in recovery mode in all cases.  The recovery mode calls SetupTPM
 * again, which executes (almost) the same sequence of operations.  There is a
 * good chance that, if recovery mode was entered because of a TPM failure, the
 * failure will repeat itself.  (In general this is impossible to guarantee
 * because we have no way of creating the exact TPM initial state at the
 * previous boot.)  In recovery mode, we ignore the failure and continue, thus
 * giving the recovery kernel a chance to fix things (that's why we don't set
 * bGlobalLock).  The choice is between a knowingly insecure device and a
 * bricked device.
 *
 * As a side note, observe that we go through considerable hoops to avoid using
 * the STCLEAR permissions for the index spaces.  We do this to avoid writing
 * to the TPM flashram at every reboot or wake-up, because of concerns about
 * the durability of the NVRAM.
 */
uint32_t tpm_setup(int s3flag)
{
	uint32_t result;

	result = tlcl_lib_init();
	if (result != TPM_SUCCESS) {
		printk(BIOS_ERR, "TPM: Can't initialize.\n");
		goto out;
	}

	/* Handle special init for S3 resume path */
	if (s3flag) {
		result = tlcl_resume();
		if (result == TPM_E_INVALID_POSTINIT)
			printk(BIOS_INFO, "TPM: Already initialized.\n");

		return TPM_SUCCESS;
	}

	result = tlcl_startup();
	if (result != TPM_SUCCESS) {
		printk(BIOS_ERR, "TPM: Can't run startup command.\n");
		goto out;
	}

	result = tlcl_assert_physical_presence();
	if (result != TPM_SUCCESS) {
		/*
		 * It is possible that the TPM was delivered with the physical
		 * presence command disabled.  This tries enabling it, then
		 * tries asserting PP again.
		 */
		result = tlcl_physical_presence_cmd_enable();
		if (result != TPM_SUCCESS) {
			printk(
			    BIOS_ERR,
			    "TPM: Can't enable physical presence command.\n");
			goto out;
		}

		result = tlcl_assert_physical_presence();
		if (result != TPM_SUCCESS) {
			printk(BIOS_ERR,
			       "TPM: Can't assert physical presence.\n");
			goto out;
		}
	}

#if IS_ENABLED(CONFIG_TPM1)
	result = tpm1_invoke_state_machine();
	if (result != TPM_SUCCESS)
		return result;
#endif

out:
	if (result != TPM_SUCCESS)
		post_code(POST_TPM_FAILURE);
	else
		printk(BIOS_INFO, "TPM: setup succeeded\n");

	return result;
}

uint32_t tpm_clear_and_reenable(void)
{
	uint32_t result;

	printk(BIOS_INFO, "TPM: Clear and re-enable\n");
	result = tlcl_force_clear();
	if (result != TPM_SUCCESS) {
		printk(BIOS_ERR, "TPM: Can't initiate a force clear.\n");
		return result;
	}

#if IS_ENABLED(CONFIG_TPM1)
	result = tlcl_set_enable();
	if (result != TPM_SUCCESS) {
		printk(BIOS_ERR, "TPM: Can't set enabled state.\n");
		return result;
	}

	result = tlcl_set_deactivated(0);
	if (result != TPM_SUCCESS) {
		printk(BIOS_ERR, "TPM: Can't set deactivated state.\n");
		return result;
	}
#endif

	return TPM_SUCCESS;
}

uint32_t tpm_extend_pcr(int pcr, uint8_t *digest, uint8_t *out_digest)
{
	if (!digest)
		return TPM_E_IOERROR;

	if (out_digest)
		return tlcl_extend(pcr, digest, out_digest);

	return tlcl_extend(pcr, digest, NULL);
}
