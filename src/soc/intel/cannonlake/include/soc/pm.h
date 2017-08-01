/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015-2016 Intel Corporation.
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

#ifndef _SOC_PM_H_
#define _SOC_PM_H_

#include <arch/acpi.h>
#include <soc/pmc.h>

#define PM1_EN			0x02
#define  PWRBTN_EN		(1 << 8)
#define  GBL_EN			(1 << 5)
#define SMI_EN			0x30
#define  ESPI_SMI_EN		(1 << 28)
#define  APMC_EN		(1 <<  5)
#define  SLP_SMI_EN		(1 <<  4)
#define  EOS			(1 <<  1)
#define  GBL_SMI_EN		(1 <<  0)


#define GPE0_EN(x)		(0x70 + ((x) * 4))
#define  PME_B0_EN		(1 << 13)

#define ENABLE_SMI_PARAMS \
	(APMC_EN | SLP_SMI_EN | GBL_SMI_EN | ESPI_SMI_EN | EOS)

struct chipset_power_state {
	uint16_t pm1_sts;
	uint16_t pm1_en;
	uint32_t pm1_cnt;
	uint16_t tco1_sts;
	uint16_t tco2_sts;
	uint32_t gpe0_sts[4];
	uint32_t gpe0_en[4];
	uint32_t gen_pmcon_a;
	uint32_t gen_pmcon_b;
	uint32_t gblrst_cause[2];
	uint32_t prev_sleep_state;
} __attribute__ ((packed));

struct chipset_power_state *fill_power_state(void);

#endif
