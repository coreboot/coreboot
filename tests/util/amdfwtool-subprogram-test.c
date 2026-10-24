/* SPDX-License-Identifier: GPL-2.0-only */

#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../util/amdfwtool/amdfwtool.h"

struct _amd_fw_entry *amd_psp_fw_table;
struct amd_bios_entry amd_bios_table[] = { { .type = AMD_BIOS_INVALID } };

static void check(bool recovery)
{
	amd_cb_config config = { .soc_id = PLATFORM_TURIN, .recovery_ab = recovery };
	char text[] = "FIRMWARE_LOCATION .\n"
		      "PSPBTLDR_AB_SUB1_FILE signed-bootloader.bin Lbb\n";
	FILE *input = fmemopen(text, sizeof(text) - 1, "r");
	amd_fw_entry *first = NULL, *second = NULL;

	assert(input);
	amd_psp_fw_table = platform_get_psp_fw_table(PLATFORM_TURIN);
	for (amd_fw_entry *entry = amd_psp_fw_table; entry->type != AMD_FW_PSP_INVALID;
	     entry++) {
		entry->filename = NULL;
		if (entry->type == AMD_FW_PSP_BOOTLOADER_AB) {
			if (entry->subprog == 0)
				first = entry;
			if (entry->subprog == 1)
				second = entry;
		}
	}
	assert(first && second);
	assert(process_config(input, &config));
	assert(fclose(input) == 0);
	assert(second->filename);
	assert(strcmp(second->filename, "./signed-bootloader.bin") == 0);
	assert(second->generate_manifest);
	assert(first->filename == NULL);
	free(second->filename);
	second->filename = NULL;
}

int main(void)
{
	check(false);
	check(true);
	puts("PASS: independent server bootloader subprogram");
	return 0;
}
