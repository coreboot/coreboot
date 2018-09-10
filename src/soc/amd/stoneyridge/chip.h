/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010-2017 Advanced Micro Devices, Inc.
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

#ifndef __STONEYRIDGE_CHIP_H__
#define __STONEYRIDGE_CHIP_H__

#include <stddef.h>
#include <stdint.h>
#include <commonlib/helpers.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <soc/gpio.h>
#include <arch/acpi_device.h>

#define MAX_NODES 1
#define MAX_DRAM_CH 1
#define MAX_DIMMS_PER_CH 2

#define STONEY_I2C_DEV_MAX 4

struct soc_amd_stoneyridge_config {
	u8 spd_addr_lookup[MAX_NODES][MAX_DRAM_CH][MAX_DIMMS_PER_CH];
	enum {
		DRAM_CONTENTS_KEEP,
		DRAM_CONTENTS_CLEAR
	} dram_clear_on_reset;

	enum {
		/* Do not enable UMA in the system. */
		UMAMODE_NONE,
		/* Enable UMA with a specific size. */
		UMAMODE_SPECIFIED_SIZE,
		/* Let AGESA determine the proper size. Non-legacy requires
		 * the resolution to be specified PLATFORM_CONFIGURATION */
		UMAMODE_AUTO_LEGACY,
		UMAMODE_AUTO_NON_LEGACY,
	} uma_mode;

	/* Used if UMAMODE_SPECIFIED_SIZE is set. */
	size_t uma_size;

	struct dw_i2c_bus_config i2c[STONEY_I2C_DEV_MAX];
	u8 stapm_percent;
	u32 stapm_time;
	u32 stapm_power;
};

typedef struct soc_amd_stoneyridge_config config_t;

extern struct device_operations pci_domain_ops;

#endif /* __STONEYRIDGE_CHIP_H__ */
