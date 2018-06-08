/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#ifndef _BAYTRAIL_RAMSTAGE_H_
#define _BAYTRAIL_RAMSTAGE_H_

#include <device/device.h>
#include <soc/intel/baytrail/chip.h>

/* The baytrail_init_pre_device() function is called prior to device
 * initialization, but it's after console and cbmem has been reinitialized. */
void baytrail_init_pre_device(struct soc_intel_baytrail_config *config);
void baytrail_init_cpus(struct device *dev);
void set_max_freq(void);
void southcluster_enable_dev(struct device *dev);
#if IS_ENABLED(CONFIG_HAVE_REFCODE_BLOB)
void baytrail_run_reference_code(void);
#else
static inline void baytrail_run_reference_code(void) {}
#endif
void baytrail_init_scc(void);
void scc_enable_acpi_mode(struct device *dev, int iosf_reg, int nvs_index);

extern struct pci_operations soc_pci_ops;

#endif /* _BAYTRAIL_RAMSTAGE_H_ */
