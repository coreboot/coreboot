/*
 * This file is part of the coreboot project.
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

#include <soc/ramstage.h>
#include <console/console.h>
#include <fsp/api.h>
#include <include/cbfs.h>

void load_logo(FSPS_UPD *supd)
{
	FSP_S_CONFIG *params = &supd->FspsConfig;

	params->LogoSize = cbfs_boot_load_file("logo.bmp", (void *)params->LogoPtr,
					       params->LogoSize, CBFS_TYPE_RAW);
	if (!params->LogoSize)
		params->LogoPtr = 0;
}
