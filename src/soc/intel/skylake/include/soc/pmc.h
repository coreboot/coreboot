/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#ifndef _SOC_PMC_H_
#define _SOC_PMC_H_

/* PCI Configuration Space (D31:F2): PMC */
#define ABASE			0x40
#define ACTL			0x44
#define  PWRM_EN		(1 << 8)
#define  ACPI_EN		(1 << 7)
#define  SCI_IRQ_SEL		(7 << 0)
#define  SCIS_IRQ9		0
#define  SCIS_IRQ10		1
#define  SCIS_IRQ11		2
#define  SCIS_IRQ20		4
#define  SCIS_IRQ21		5
#define  SCIS_IRQ22		6
#define  SCIS_IRQ23		7
#define PWRMBASE		0x48
#define GEN_PMCON_A		0xa0
#define  SMI_LOCK		(1 << 4)
#define GEN_PMCON_B		0xa4
#define  SLP_STR_POL_LOCK	(1 << 18)
#define  ACPI_BASE_LOCK		(1 << 17)
#define  SUS_PWR_FLR		(1 << 14)
#define  HOST_RST_STS		(1 << 9)
#define  RTC_BATTERY_DEAD	(1 << 2)
#define  PWR_FLR		(1 << 1)
#define  SLEEP_AFTER_POWER_FAIL	(1 << 0)
#define ETR3			0xac
#define  ETR3_CF9LOCK		(1 << 31)
#define  ETR3_CF9GR		(1 << 20)

/* Memory mapped IO registers in PMC */
#define S3_PWRGATE_POL		0x28
#define  S3DC_GATE_SUS		(1 << 1)
#define  S3AC_GATE_SUS		(1 << 0)
#define S4_PWRGATE_POL		0x2c
#define  S4DC_GATE_SUS		(1 << 1)
#define  S4AC_GATE_SUS		(1 << 0)
#define S5_PWRGATE_POL		0x30
#define  S5DC_GATE_SUS		(1 << 15)
#define  S5AC_GATE_SUS		(1 << 14)
#define PMSYNC_TPR_CFG		0xc4
#define  PMSYNC_LOCK		(1 << 31)
#define GBLRST_CAUSE0		0x124
#define GBLRST_CAUSE1		0x128


/* TCO registers and fields live behind TCOBASE I/O bar in SMBus device. */
#define TCO1_STS			0x04
#define TCO2_STS			0x06
#define  TCO2_STS_SECOND_TO		0x02
#define  TCO2_STS_BOOT		0x04
#define TCO1_CNT			0x08
#define  TCO_LOCK			(1 << 12)
#define  TCO_TMR_HLT			(1 << 11)

#endif
