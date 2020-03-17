/*
 * This file is part of the coreboot project.
 *
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

#ifndef SOC_APOLLOLAKE_SYSTEMAGENT_H
#define SOC_APOLLOLAKE_SYSTEMAGENT_H

#include <intelblocks/systemagent.h>

/* RAPL Package Power Limit register under MCHBAR. */
#define PUNIT_THERMAL_DEVICE_IRQ		0x700C
#define PUINT_THERMAL_DEVICE_IRQ_VEC_NUMBER	0x18
#define PUINT_THERMAL_DEVICE_IRQ_LOCK		0x80000000
#define BIOS_RESET_CPL		0x7078
#define   PCODE_INIT_DONE	(1 << 8)
#define MCHBAR_RAPL_PPL		0x70A8
#define CORE_DISABLE_MASK	0x7168
#define CAPID0_A		0xE4
#define   VTD_DISABLE		(1 << 23)
#define DEFVTBAR		0x6c80
#define GFXVTBAR		0x6c88
#define   VTBAR_ENABLED		0x01
#define VTBAR_MASK		0xfffffff000ull
#define VTBAR_SIZE		0x1000

#endif /* SOC_APOLLOLAKE_SYSTEMAGENT_H */
