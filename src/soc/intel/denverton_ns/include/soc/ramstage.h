/*
 * This file is part of the coreboot project.
 *
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
 */

#ifndef _DENVERTON_NS_SOC_RAMSTAGE_H_
#define _DENVERTON_NS_SOC_RAMSTAGE_H_

#include <device/device.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <memory_info.h>

void denverton_init_cpus(device_t dev);
void mainboard_silicon_init_params(FSPS_UPD *params);
void southcluster_enable_dev(device_t dev);
void mainboard_add_dimm_info(struct memory_info *mem_info, int channel,
			     int dimm, int index);

extern struct pci_operations soc_pci_ops;

#endif
