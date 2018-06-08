/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 */

#ifndef SOC_INTEL_COMMON_BLOCK_CHIP_H
#define SOC_INTEL_COMMON_BLOCK_CHIP_H

#include <intelblocks/gspi.h>
#include <drivers/i2c/designware/dw_i2c.h>

enum {
	CHIPSET_LOCKDOWN_FSP = 0, /* FSP handles locking per UPDs */
	CHIPSET_LOCKDOWN_COREBOOT, /* coreboot handles locking */
};

/*
 * This structure will hold data required by common blocks.
 * These are soc specific configurations which will be filled by soc.
 * We'll fill this structure once during init and use the data in common block.
 */
struct soc_intel_common_config {
	int chipset_lockdown;
	struct gspi_cfg gspi[CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX];
	struct dw_i2c_bus_config i2c[CONFIG_SOC_INTEL_I2C_DEV_MAX];
};

/* This function to retrieve soc config structure required by common code */
const struct soc_intel_common_config *chip_get_common_soc_structure(void);

#endif /* SOC_INTEL_COMMON_BLOCK_CHIP_H */
