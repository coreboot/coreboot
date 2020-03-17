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

#ifndef _SOC_RAMSTAGE_H_
#define _SOC_RAMSTAGE_H_

#include <device/device.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <memory_info.h>

void xeon_sp_init_cpus(struct device *dev);
void mainboard_silicon_init_params(FSPS_UPD *params);

extern struct pci_operations soc_pci_ops;

#endif
