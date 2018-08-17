/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Lance Zhao <lijian.zhao@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * NOTE: The layout of the global_nvs_t structure below must match the layout
 * in soc/soc/amd/stoneyridge/acpi/globalnvs.asl !!!
 *
 */

#ifndef __SOC_STONEYRIDGE_NVS_H__
#define __SOC_STONEYRIDGE_NVS_H__

#include <commonlib/helpers.h>
#include <compiler.h>
#include <stdint.h>
#include <vendorcode/google/chromeos/gnvs.h>

typedef struct global_nvs_t {
	/* Miscellaneous */
	uint8_t		pcnt; /* 0x00 - Processor Count */
	uint8_t		ppcm; /* 0x01 - Max PPC State */
	uint8_t		lids; /* 0x02 - LID State */
	uint8_t		pwrs; /* 0x03 - AC Power State */
	uint8_t		dpte; /* 0x04 - Enable DPTF */
	uint32_t	cbmc; /* 0x05 - 0x08 - coreboot Memory Console */
	uint64_t	pm1i; /* 0x09 - 0x10 - System Wake Source - PM1 Index */
	uint64_t	gpei; /* 0x11 - 0x18 - GPE Wake Source */
	uint64_t	nhla; /* 0x19 - 0x20 - NHLT Address */
	uint32_t	nhll; /* 0x21 - 0x24 - NHLT Length */
	uint32_t	prt0; /* 0x25 - 0x28 - PERST_0 Address */
	uint8_t		scdp; /* 0x29 - SD_CD GPIO portid */
	uint8_t		scdo; /* 0x2A - GPIO pad offset relative to the community */
	uint8_t		tmps; /* 0x2B - Temperature Sensor ID */
	uint8_t		tlvl; /* 0x2C - Throttle Level Limit */
	uint8_t		flvl; /* 0x2D - Current FAN Level */
	uint8_t		tcrt; /* 0x2E - Critical Threshold */
	uint8_t		tpsv; /* 0x2F - Passive Threshold */
	uint8_t		tmax; /* 0x30 - CPU Tj_max */
	uint8_t		unused[207];

	/* ChromeOS specific (0x100 - 0xfff) */
	chromeos_acpi_t chromeos;
} __packed global_nvs_t;
check_member(global_nvs_t, chromeos, GNVS_CHROMEOS_ACPI_OFFSET);

#endif /* __SOC_STONEYRIDGE_NVS_H__ */
