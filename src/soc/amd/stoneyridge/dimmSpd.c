/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 - 2016 Advanced Micro Devices, Inc.
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

#include <agesawrapper.h>
#include <device/pci_def.h>
#include <device/device.h>
#include <soc/pci_devs.h>
#include <stdlib.h>
#include "chip.h"
#include <dimmSpd.h>

AGESA_STATUS AmdMemoryReadSPD(UINT32 unused1, UINTN unused2,
						AGESA_READ_SPD_PARAMS *info)
{
	uint8_t spdAddress;
	DEVTREE_CONST struct device *dev = dev_find_slot(0, DCT_DEVFN);
	DEVTREE_CONST struct soc_amd_stoneyridge_config *conf = dev->chip_info;

	if ((dev == 0) || (conf == 0))
		return AGESA_ERROR;
	if (info->SocketId >= ARRAY_SIZE(conf->spdAddrLookup))
		return AGESA_ERROR;
	if (info->MemChannelId >= ARRAY_SIZE(conf->spdAddrLookup[0]))
		return AGESA_ERROR;
	if (info->DimmId >= ARRAY_SIZE(conf->spdAddrLookup[0][0]))
		return AGESA_ERROR;
	spdAddress = conf->spdAddrLookup
		[info->SocketId][info->MemChannelId][info->DimmId];
	if (spdAddress == 0)
		return AGESA_ERROR;
	int err = sb_readSpd(spdAddress, (void *)info->Buffer, 128);
	if (err)
		return AGESA_ERROR;
	return AGESA_SUCCESS;
}
