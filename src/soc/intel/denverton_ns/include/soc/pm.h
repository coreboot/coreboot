/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 - 2017 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _DENVERTON_NS_PM_H_
#define _DENVERTON_NS_PM_H_

#include <arch/io.h>
#include <soc/pmc.h>
#include <arch/acpi.h>

#define GPE_MAX 127

struct chipset_power_state {
	uint16_t pm1_sts;
	uint16_t pm1_en;
	uint32_t pm1_cnt;
	uint16_t tco1_sts;
	uint16_t tco2_sts;
	uint32_t gpe0_sts[GPE0_REG_MAX];
	uint32_t gpe0_en[GPE0_REG_MAX];
	uint32_t gen_pmcon_a;
	uint32_t gen_pmcon_b;
	uint32_t gblrst_cause[2];
	uint32_t prev_sleep_state;
} __attribute__((packed));

struct chipset_power_state *fill_power_state(void);

/* Power Management Utility Functions. */
uint32_t clear_smi_status(void);
uint16_t clear_pm1_status(void);
uint32_t clear_tco_status(void);
uint32_t clear_gpe_status(void);
void clear_pmc_status(void);
void enable_smi(uint32_t mask);
void disable_smi(uint32_t mask);
void enable_pm1(uint16_t events);
void enable_pm1_control(uint32_t mask);
void disable_pm1_control(uint32_t mask);
void enable_gpe(uint32_t mask);
void disable_gpe(uint32_t mask);
void disable_all_gpe(void);

#endif /* _DENVERTON_NS_PM_H_ */
