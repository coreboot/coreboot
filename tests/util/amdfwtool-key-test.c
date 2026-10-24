/* SPDX-License-Identifier: GPL-2.0-only */

#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../util/amdfwtool/amdfwtool.h"

static amd_fw_entry test_psp_fw_table[] = {
	{ .type = AMD_FW_PSP_SECURED_DEBUG },
	{ .type = AMD_FW_PSP_DEBUG_UNLOCK },
	{ .type = AMD_FW_PSP_INVALID },
};
amd_fw_entry *amd_psp_fw_table = test_psp_fw_table;
struct amd_bios_entry amd_bios_table[] = { { .type = AMD_BIOS_INVALID } };

static void check_capability(enum platform soc, bool unlock, bool keep_auth,
			     const char *keyword, unsigned int slot, bool present)
{
	amd_cb_config config = {
		.soc_id = soc,
		.unlock_secure_supported = unlock || keep_auth,
	};
	char text[160];
	int length = snprintf(text, sizeof(text),
			      "FIRMWARE_LOCATION .\n%s signed-firmware.bin Lb\n", keyword);
	assert(length > 0 && (size_t)length < sizeof(text));
	FILE *input = fmemopen(text, length, "r");
	assert(input);

	for (unsigned int i = 0; i < 2; i++) {
		amd_psp_fw_table[i].filename = NULL;
		amd_psp_fw_table[i].level = 0;
	}
	assert(process_config(input, &config));
	assert(fclose(input) == 0);
	assert((amd_psp_fw_table[slot].filename != NULL) == present);
	if (present) {
		assert(strcmp(amd_psp_fw_table[slot].filename, "./signed-firmware.bin") == 0);
		assert(amd_psp_fw_table[slot].level == PSP_BOTH);
		free(amd_psp_fw_table[slot].filename);
		amd_psp_fw_table[slot].filename = NULL;
	}
	assert(amd_psp_fw_table[1 - slot].filename == NULL);
	assert(config.unlock_secure_supported == (unlock || keep_auth));
}

static void check(enum platform soc, bool unlock, const char *keyword, unsigned int slot,
		  bool present)
{
	check_capability(soc, unlock, platform_needs_auth_files(soc), keyword, slot, present);
}

int main(void)
{
	check(PLATFORM_TURIN, false, "PSP_SEC_DBG_KEY_FILE", 0, true);
	check(PLATFORM_TURIN, true, "PSP_SEC_DBG_KEY_FILE", 0, true);
	check(PLATFORM_CEZANNE, false, "PSP_SEC_DBG_KEY_FILE", 0, false);
	check(PLATFORM_CEZANNE, true, "PSP_SEC_DBG_KEY_FILE", 0, true);
	check(PLATFORM_CEZANNE, false, "PSPSECUREDEBUG_FILE", 0, true);
	check(PLATFORM_TURIN, false, "PSP_SEC_DEBUG_FILE", 1, true);
	check(PLATFORM_TURIN, true, "PSP_SEC_DEBUG_FILE", 1, true);
	check(PLATFORM_CEZANNE, false, "PSP_SEC_DEBUG_FILE", 1, false);
	check(PLATFORM_CEZANNE, true, "PSP_SEC_DEBUG_FILE", 1, true);
	check(PLATFORM_GENOA, false, "PSP_SEC_DBG_KEY_FILE", 0, false);
	check(PLATFORM_GENOA, false, "PSP_SEC_DEBUG_FILE", 1, false);
	/* The parser follows the feature, independently of the platform name. */
	check_capability(PLATFORM_CEZANNE, false, true, "PSP_SEC_DBG_KEY_FILE", 0, true);
	check_capability(PLATFORM_CEZANNE, false, true, "PSP_SEC_DEBUG_FILE", 1, true);
	check_capability(PLATFORM_TURIN, false, false, "PSP_SEC_DBG_KEY_FILE", 0, false);
	check_capability(PLATFORM_TURIN, false, false, "PSP_SEC_DEBUG_FILE", 1, false);
	puts("PASS: authentication files and secure-debug policy");
	return 0;
}
