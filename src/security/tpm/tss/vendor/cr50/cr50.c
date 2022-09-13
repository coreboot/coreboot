/* SPDX-License-Identifier: BSD-3-Clause */

#include <console/console.h>
#include <endian.h>
#include <halt.h>
#include <vb2_api.h>
#include <security/tpm/tis.h>
#include <security/tpm/tss.h>

#include "../../tcg-2.0/tss_marshaling.h"

uint32_t tlcl_cr50_enable_nvcommits(void)
{
	uint16_t sub_command = TPM2_CR50_SUB_CMD_NVMEM_ENABLE_COMMITS;
	struct tpm2_response *response;

	printk(BIOS_INFO, "Enabling cr50 nvmem commits\n");

	response = tpm_process_command(TPM2_CR50_VENDOR_COMMAND, &sub_command);

	if (!response || (response && response->hdr.tpm_code)) {
		if (response)
			printk(BIOS_INFO, "%s: failed %x\n", __func__,
			       response->hdr.tpm_code);
		else
			printk(BIOS_INFO, "%s: failed\n", __func__);
		return TPM_E_IOERROR;
	}
	return TPM_SUCCESS;
}

uint32_t tlcl_cr50_enable_update(uint16_t timeout_ms,
				 uint8_t *num_restored_headers)
{
	struct tpm2_response *response;
	uint16_t command_body[] = {
		TPM2_CR50_SUB_CMD_TURN_UPDATE_ON, timeout_ms
	};

	printk(BIOS_INFO, "Checking cr50 for pending updates\n");

	response = tpm_process_command(TPM2_CR50_VENDOR_COMMAND, command_body);

	if (!response || response->hdr.tpm_code)
		return TPM_E_IOERROR;

	*num_restored_headers = response->vcr.num_restored_headers;
	return TPM_SUCCESS;
}

uint32_t tlcl_cr50_get_recovery_button(uint8_t *recovery_button_state)
{
	struct tpm2_response *response;
	uint16_t sub_command = TPM2_CR50_SUB_CMD_GET_REC_BTN;

	printk(BIOS_INFO, "Checking cr50 for recovery request\n");

	response = tpm_process_command(TPM2_CR50_VENDOR_COMMAND, &sub_command);

	if (!response || response->hdr.tpm_code)
		return TPM_E_IOERROR;

	*recovery_button_state = response->vcr.recovery_button_state;
	return TPM_SUCCESS;
}

uint32_t tlcl_cr50_get_tpm_mode(uint8_t *tpm_mode)
{
	struct tpm2_response *response;
	uint16_t mode_command = TPM2_CR50_SUB_CMD_TPM_MODE;
	*tpm_mode = TPM_MODE_INVALID;

	printk(BIOS_INFO, "Reading cr50 TPM mode\n");

	response = tpm_process_command(TPM2_CR50_VENDOR_COMMAND, &mode_command);

	if (!response)
		return TPM_E_IOERROR;

	if (response->hdr.tpm_code == VENDOR_RC_INTERNAL_ERROR) {
		/*
		 * The Cr50 returns VENDOR_RC_INTERNAL_ERROR iff the key ladder
		 * is disabled. The Cr50 requires a reboot to re-enable the key
		 * ladder.
		 */
		return TPM_E_MUST_REBOOT;
	}

	if (response->hdr.tpm_code == VENDOR_RC_NO_SUCH_COMMAND ||
	    response->hdr.tpm_code == VENDOR_RC_NO_SUCH_SUBCOMMAND) {
		/*
		 * Explicitly inform caller when command is not supported
		 */
		return TPM_E_NO_SUCH_COMMAND;
	}

	if (response->hdr.tpm_code) {
		/* Unexpected return code from Cr50 */
		return TPM_E_IOERROR;
	}

	/* TPM command completed without error */
	*tpm_mode = response->vcr.tpm_mode;

	return TPM_SUCCESS;
}

uint32_t tlcl_cr50_get_boot_mode(uint8_t *boot_mode)
{
	struct tpm2_response *response;
	uint16_t mode_command = TPM2_CR50_SUB_CMD_GET_BOOT_MODE;

	printk(BIOS_DEBUG, "Reading cr50 boot mode\n");

	response = tpm_process_command(TPM2_CR50_VENDOR_COMMAND, &mode_command);

	if (!response)
		return TPM_E_IOERROR;

	if (response->hdr.tpm_code == VENDOR_RC_NO_SUCH_COMMAND ||
	    response->hdr.tpm_code == VENDOR_RC_NO_SUCH_SUBCOMMAND)
		/* Explicitly inform caller when command is not supported */
		return TPM_E_NO_SUCH_COMMAND;

	if (response->hdr.tpm_code)
		/* Unexpected return code from Cr50 */
		return TPM_E_IOERROR;

	*boot_mode = response->vcr.boot_mode;

	return TPM_SUCCESS;
}

uint32_t tlcl_cr50_immediate_reset(uint16_t timeout_ms)
{
	struct tpm2_response *response;
	uint16_t reset_command_body[] = {
		TPM2_CR50_SUB_CMD_IMMEDIATE_RESET, timeout_ms};

	/*
	 * Issue an immediate reset to the Cr50.
	 */
	printk(BIOS_INFO, "Issuing cr50 reset\n");
	response = tpm_process_command(TPM2_CR50_VENDOR_COMMAND,
				       &reset_command_body);

	if (!response)
		return TPM_E_IOERROR;

	return TPM_SUCCESS;
}

uint32_t tlcl_cr50_reset_ec(void)
{
	struct tpm2_response *response;
	uint16_t reset_cmd = TPM2_CR50_SUB_CMD_RESET_EC;

	printk(BIOS_DEBUG, "Issuing EC reset\n");

	response = tpm_process_command(TPM2_CR50_VENDOR_COMMAND, &reset_cmd);

	if (!response)
		return TPM_E_IOERROR;

	if (response->hdr.tpm_code == VENDOR_RC_NO_SUCH_COMMAND ||
	    response->hdr.tpm_code == VENDOR_RC_NO_SUCH_SUBCOMMAND)
		/* Explicitly inform caller when command is not supported */
		return TPM_E_NO_SUCH_COMMAND;

	if (response->hdr.tpm_code)
		/* Unexpected return code from Cr50 */
		return TPM_E_IOERROR;

	printk(BIOS_DEBUG, "EC reset coming up...\n");
	halt();

	return TPM_SUCCESS;
}
