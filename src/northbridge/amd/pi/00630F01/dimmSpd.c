/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <device/pci_def.h>
#include <device/device.h>

/* warning: Porting.h includes an open #pragma pack(1) */
#include "Porting.h"
#include "AGESA.h"
#include "amdlib.h"
#include "chip.h"

#include "northbridge/amd/pi/dimmSpd.h"

#define DIMENSION(array)(sizeof (array)/ sizeof (array [0]))

AGESA_STATUS AmdMemoryReadSPD (UINT32 unused1, UINT32 unused2, AGESA_READ_SPD_PARAMS *info)
{
	int spdAddress;
	ROMSTAGE_CONST struct device *dev = dev_find_slot(0, PCI_DEVFN(0x18, 2));
	ROMSTAGE_CONST struct northbridge_amd_pi_00630F01_config *config = dev->chip_info;

	if ((dev == 0) || (config == 0))
		return AGESA_ERROR;

	if (info->SocketId     >= DIMENSION(config->spdAddrLookup      ))
		return AGESA_ERROR;
	if (info->MemChannelId >= DIMENSION(config->spdAddrLookup[0]   ))
		return AGESA_ERROR;
	if (info->DimmId       >= DIMENSION(config->spdAddrLookup[0][0]))
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
