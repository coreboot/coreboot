/*
 * Copyright 2016 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <arch/early_variables.h>
#include <console/console.h>
#include <endian.h>
#include <string.h>
#include <vb2_api.h>
#include <security/tpm/tis.h>
#include <security/tpm/tss.h>

#include "../../tcg-2.0/tss_marshaling.h"

uint32_t tlcl_cr50_enable_nvcommits(void)
{
	uint16_t sub_command = TPM2_CR50_SUB_CMD_NVMEM_ENABLE_COMMITS;
	struct tpm2_response *response;

	printk(BIOS_INFO, "Enabling cr50 nvmem commmits\n");

	response = tpm_process_command(TPM2_CR50_VENDOR_COMMAND, &sub_command);

	if (response == NULL || (response && response->hdr.tpm_code)) {
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
		return TPM_E_INTERNAL_INCONSISTENCY;

	*num_restored_headers = response->vcr.num_restored_headers;
	return TPM_SUCCESS;
}
