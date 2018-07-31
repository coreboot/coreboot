/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Google Inc.
 * Copyright (C) 2018 Intel Corporation.
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

#include <baseboard/variants.h>
#include <chip.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#define PL2_AML	18
#define PL2_KBL	15

static uint32_t get_pl2(void)
{
	uint16_t id;
	id = pci_read_config16(SA_DEV_IGD, PCI_DEVICE_ID);
	/* Assume we only have KLB-Y and AML-Y SKUs */
	if (id == PCI_DEVICE_ID_INTEL_KBL_GT2_SULXM)
		return PL2_KBL;

	return PL2_AML;
}

/* Override dev tree settings per board */
void variant_devtree_update(void)
{
	struct device *root = SA_DEV_ROOT;
	config_t *cfg = root->chip_info;

	/* Update PL2 based on CPU */
	cfg->tdp_pl2_override = get_pl2();
}
