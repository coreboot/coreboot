/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
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

#ifndef _SOC_APOLLOLAKE_PM_H_
#define _SOC_APOLLOLAKE_PM_H_

/* ACPI_BASE_ADDRESS / PMBASE */

#define PM1_STS			0x00
#define PM1_EN			0x02
#define PM1_CNT			0x04
#define PM1_TMR			0x08
#define SMI_EN			0x40
#define SMI_STS			0x44
#define GPE_CNTL		0x50
#define DEVACT_STS		0x4c

#define GPE0_REG_MAX		4
#define GPE0_STS(x)		(0x20 + (x * 4))
#define GPE0_EN(x)		(0x30 + (x * 4))

#endif	/* _SOC_APOLLOLAKE_PM_H_ */
