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

#ifndef _SOC_P2SB_H_
#define _SOC_P2SB_H_

#define HPTC_OFFSET		0x60
#define HPTC_ADDR_ENABLE_BIT	(1 << 7)

#define PCH_P2SB_IBDF			0x6c
#define PCH_P2SB_HBDF			0x70

#define PCH_P2SB_EPMASK0		0xB0

#define PCH_PWRM_ACPI_TMR_CTL		0xFC

#endif /* _SOC_P2SB_H_ */
