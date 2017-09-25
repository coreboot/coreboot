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

#include <stdint.h>

#define MAX_NODES 1
#define MAX_DRAM_CH 1
#define MAX_DIMMS_PER_CH 2

struct soc_amd_stoneyridge_config {
	u8 spdAddrLookup[MAX_NODES][MAX_DRAM_CH][MAX_DIMMS_PER_CH];
};

typedef struct soc_amd_stoneyridge_config config_t;

extern struct device_operations pci_domain_ops;

#endif /* __STONEYRIDGE_CHIP_H__ */
