/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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

#include <device/device.h>

/* The broadwell_de_init_pre_device() function is called prior to device
 * initialization, but it's after console and cbmem has been reinitialized. */
void broadwell_de_init_pre_device(void);
void broadwell_de_init_cpus(struct device *dev);
void southcluster_enable_dev(struct device *dev);

extern struct pci_operations soc_pci_ops;

#endif /* _SOC_RAMSTAGE_H_ */
