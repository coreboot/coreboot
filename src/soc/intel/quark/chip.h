/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#include <stdint.h>
#include <fsp/util.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>

struct soc_intel_quark_config {
	/*
	 * MemoryInit:
	 *
	 * The following fields come from FspUpdVpd.h and are defined as PCDs
         * for the FSP binary.  Data for these fields comes from the board's
	 * devicetree.cb file which gets processed into static.c and then
	 * built into the coreboot image.  The fields below contain retain
	 * the FSP PCD field name.
	 */
	UINT16 PcdSmmTsegSize;
};

extern struct chip_operations soc_ops;

#endif
