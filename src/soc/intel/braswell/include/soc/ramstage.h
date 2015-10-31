/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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

#ifndef _SOC_RAMSTAGE_H_
#define _SOC_RAMSTAGE_H_

#include <chip.h>
#include <device/device.h>
#include <fsp/ramstage.h>

/*
 * The soc_init_pre_device() function is called prior to device
 * initialization, but it's after console and cbmem has been reinitialized.
 */
void soc_init_pre_device(struct soc_intel_braswell_config *config);
void soc_init_cpus(device_t dev);
void set_max_freq(void);
void southcluster_enable_dev(device_t dev);
void scc_enable_acpi_mode(device_t dev, int iosf_reg, int nvs_index);

extern struct pci_operations soc_pci_ops;

#endif /* _SOC_RAMSTAGE_H_ */
