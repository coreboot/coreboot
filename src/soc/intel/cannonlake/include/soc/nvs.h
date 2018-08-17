/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
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

#ifndef _SOC_NVS_H_
#define _SOC_NVS_H_

#include <commonlib/helpers.h>
#include <compiler.h>
#include <vendorcode/google/chromeos/gnvs.h>

typedef struct global_nvs_t {
	/* Miscellaneous */
	u16	osys; /* 0x00 - 0x01 Operating System */
	u8	smif; /* 0x02 - SMI function call ("TRAP") */
	u8      pcnt; /* 0x03 - Processor Count */
	u8	ppcm; /* 0x04 - Max PPC State */
	u8	tlvl; /* 0x05 - Throttle Level Limit */
	u8	lids; /* 0x06 - LID State */
	u8	pwrs; /* 0x07 - AC Power State */
	u32	cbmc; /* 0x08 - 0xb AC Power State */
	u64	pm1i; /* 0x0c - 0x13 PM1 wake status bit */
	u64	gpei; /* 0x14 - 0x1b GPE wake status bit */
	u8	dpte; /* 0x1c - Enable DPTF */
	u64	nhla; /* 0x1d - 0x24 NHLT Address */
	u32	nhll; /* 0x25 - 0x28 NHLT Length */
	u16	cid1; /* 0x29 - 0x2a Wifi Country Identifier */
	u16	u2we; /* 0x2b - 0x2c USB2 Wake Enable Bitmap */
	u16	u3we; /* 0x2d - 0x2e USB3 Wake Enable Bitmap */
	u8	uior; /* 0x2f - UART debug controller init on S3 resume */
	u8	unused[208];

	/* ChromeOS specific (0x100 - 0xfff) */
	chromeos_acpi_t chromeos;
}  __packed global_nvs_t;
check_member(global_nvs_t, chromeos, GNVS_CHROMEOS_ACPI_OFFSET);


#endif
