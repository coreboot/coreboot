/*
 * This file is part of the coreboot project.
 *
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

#include <device/pci_def.h>
#include <device/device.h>

/* warning: Porting.h includes an open #pragma pack(1) */
#include <Porting.h>
#include <AGESA.h>
#include "chip.h"
#include <northbridge/amd/pi/dimmSpd.h>

AGESA_STATUS AmdMemoryReadSPD (UINT32 unused1, UINTN unused2, AGESA_READ_SPD_PARAMS *info)
{
	int spdAddress;
	DEVTREE_CONST struct device *dev = pcidev_on_root(0x18, 2);

	if (dev == NULL)
		return AGESA_ERROR;

	DEVTREE_CONST struct northbridge_amd_pi_00660F01_config *config = dev->chip_info;

	if (config == NULL)
		return AGESA_ERROR;

	if (info->SocketId >= ARRAY_SIZE(config->spdAddrLookup))
		return AGESA_ERROR;
	if (info->MemChannelId >= ARRAY_SIZE(config->spdAddrLookup[0]))
		return AGESA_ERROR;
	if (info->DimmId >= ARRAY_SIZE(config->spdAddrLookup[0][0]))
		return AGESA_ERROR;
	spdAddress = config->spdAddrLookup
		[info->SocketId] [info->MemChannelId] [info->DimmId];
	if (spdAddress == 0)
		return AGESA_ERROR;
	int err = hudson_readSpd(spdAddress, (void *) info->Buffer, 128);
	if (err)
		return AGESA_ERROR;
	return AGESA_SUCCESS;
}
