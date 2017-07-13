/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
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

#include <console/console.h>
#include <fsp/bootblock.h>
#include <fsp/util.h>

static void fill_temp_ram_init_params(FSP_TEMP_RAM_INIT_PARAMS *params)
{
	params->MicrocodeRegionBase = CONFIG_CPU_MICROCODE_CBFS_LOC;
	params->MicrocodeRegionLength = CONFIG_CPU_MICROCODE_CBFS_LEN;
	params->CodeRegionBase = 0xFFFFFFFF - CONFIG_ROM_SIZE + 1;
	params->CodeRegionLength = CONFIG_ROM_SIZE;
}

void bootblock_fsp_temp_ram_init(void)
{
	FSP_TEMP_RAM_INIT fsp_temp_ram_init;
	FSP_TEMP_RAM_INIT_PARAMS temp_ram_init_params;
	FSP_INFO_HEADER *fih;
	EFI_STATUS status;

	/* Locate the FSP header */
	fih = find_fsp(CONFIG_FSP_LOC);
	/* Check the FSP header */
	if (((uintptr_t)fih >= ERROR_NO_FV_SIG) &&
	    ((uintptr_t)fih <= ERROR_FSP_REV_MISMATCH)) {
		printk(BIOS_ERR, "FSP header error %p, ", fih);
		fih = NULL;
	}
	if (fih == NULL)
		die("FSP_INFO_HEADER not set!\n");

	fill_temp_ram_init_params(&temp_ram_init_params);

	/* Perform Temp RAM Init */
	printk(BIOS_DEBUG, "Calling FspTempRamInit\n");
	post_code(POST_FSP_TEMP_RAM_INIT);
	fsp_temp_ram_init = (FSP_TEMP_RAM_INIT)(fih->ImageBase
			+ fih->TempRamInitEntryOffset);
	status = fsp_temp_ram_init(&temp_ram_init_params);
	if (status != FSP_SUCCESS)
		die("FspTempRamInit failed. Giving up.");

	printk(BIOS_DEBUG, "FspTempRamInit returned 0x%08x\n", status);
}
