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
 * in soc/intel/apollolake/acpi/globalnvs.asl !!!
 *
 */

#ifndef _SOC_APOLLOLAKE_NVS_H_
#define _SOC_APOLLOLAKE_NVS_H_

#include <vendorcode/google/chromeos/gnvs.h>

struct global_nvs_t {
	/* Miscellaneous */
	uint8_t	pcnt; /* 0x00 - Processor Count */
	uint8_t	ppcm; /* 0x01 - Max PPC State */
	uint8_t	lids; /* 0x02 - LID State */
	uint8_t	pwrs; /* 0x03 - AC Power State */
	uint8_t	dpte; /* 0x04 - Enable DPTF */
	uint32_t	cbmc; /* 0x05 - 0x08 - Coreboot Memory Console */
	uint8_t	unused[247];

	/* ChromeOS specific (0x100 - 0xfff) */
	chromeos_acpi_t chromeos;
} __attribute__((packed));

#endif	/* _SOC_APOLLOLAKE_NVS_H_ */
