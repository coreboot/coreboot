/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
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

#include <arch/cpu.h>
#include <chip.h>
#include <device/device.h>
#include <intelblocks/mp_init.h>

#define PL2_I7_SKU	44
#define PL2_DEFAULT	29

static uint32_t nami_get_pl2(void)
{
	struct cpuid_result cpuidr;

	cpuidr = cpuid(1);
	if (cpuidr.eax == CPUID_KABYLAKE_Y0)
		return PL2_I7_SKU;

	return PL2_DEFAULT;
}

static void nami_enable(device_t dev)
{
	struct device *root = SA_DEV_ROOT;
	config_t *conf = root->chip_info;

	if (!conf)
		return;

	conf->tdp_pl2_override = nami_get_pl2();
}

struct chip_operations nami_ops = {
	.enable_dev = nami_enable,
};
