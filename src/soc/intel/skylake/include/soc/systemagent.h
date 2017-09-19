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

#ifndef SOC_SKYLAKE_SYSTEMAGENT_H
#define SOC_SKYLAKE_SYSTEMAGENT_H

#include <intelblocks/systemagent.h>

#define SA_IGD_OPROM_VENDEV	0x80860406

/* Device 0:0.0 PCI configuration space */

#define EPBAR		0x40
#define DMIBAR		0x68
#define SMRAM		0x88	/* System Management RAM Control */
#define  D_OPEN		(1 << 6)
#define  D_CLS		(1 << 5)
#define  D_LCK		(1 << 4)
#define  G_SMRAME	(1 << 3)
#define  C_BASE_SEG	((0 << 2) | (1 << 1) | (0 << 0))
#define CAPID0_A	0xe4
#define  VTD_DISABLE	(1 << 23)

#define BIOS_RESET_CPL		0x5da8
#define GFXVTBAR		0x5400
#define EDRAMBAR		0x5408
#define VTVC0BAR		0x5410
#define GDXCBAR			0x5420

#define MCH_PKG_POWER_LIMIT_LO	0x59a0
#define MCH_PKG_POWER_LIMIT_HI	0x59a4
#define MCH_DDR_POWER_LIMIT_LO	0x58e0
#define MCH_DDR_POWER_LIMIT_HI	0x58e4

bool soc_is_vtd_capable(void);

static const struct sa_mmio_descriptor soc_vtd_resources[] = {
	{ GFXVTBAR, GFXVT_BASE_ADDRESS, GFXVT_BASE_SIZE, "GFXVTBAR" },
	{ VTVC0BAR, VTVC0_BASE_ADDRESS, VTVC0_BASE_SIZE, "VTVC0BAR" },
};

#endif
