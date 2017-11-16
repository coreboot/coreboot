/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012, 2017 Advanced Micro Devices, Inc.
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

#ifndef __DIMMSPD_H__
#define __DIMMSPD_H__

AGESA_STATUS
AmdMemoryReadSPD(IN UINT32 Func, IN UINTN Data,
				IN OUT AGESA_READ_SPD_PARAMS *SpdData);

int sb_read_spd(uint8_t spdAddress, char *buf, size_t len);

#endif
