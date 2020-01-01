/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
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

#ifndef SOC_INTEL_COMMON_BLOCK_SMBUS__LIB_H
#define SOC_INTEL_COMMON_BLOCK_SMBUS__LIB_H

#include <stdint.h>

/* SMBus IO Base Address */
#define SMBUS_IO_BASE	0xefa0

/* PCI Configuration Space : SMBus */
#define HOSTC	0x40
#define HST_EN	(1 << 0)
/* SMBus I/O bits. */
#define SMBHSTSTAT	0x0
#define SMBHSTCTL	0x2
#define SMBHSTCMD	0x3
#define SMBXMITADD	0x4
#define SMBHSTDAT0	0x5

#define SMBUS_TIMEOUT	15	/* 15ms */

#endif	/* SOC_INTEL_COMMON_BLOCK_SMBUS__LIB_H */
