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

#ifndef _SOC_CHIP_H_
#define _SOC_CHIP_H_

#include <arch/acpi.h>

/* The devicetree parser expects chip.h to reside directly in the path
 * specified by the devicetree. */

struct soc_intel_fsp_broadwell_de_config {
	/* PCIe completion timeout value */
	int pcie_compltoval;
};

typedef struct soc_intel_fsp_broadwell_de_config config_t;

extern struct chip_operations soc_intel_fsp_broadwell_de_ops;
#endif /* _SOC_CHIP_H_ */
