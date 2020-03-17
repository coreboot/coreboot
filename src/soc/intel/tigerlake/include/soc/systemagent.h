/*
 * This file is part of the coreboot project.
 *
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

/*
 * This file is created based on Intel Tiger Lake Processor SA Datasheet
 * Document number: 571131
 * Chapter number: 3
 */

#ifndef SOC_TIGERLAKE_SYSTEMAGENT_H
#define SOC_TIGERLAKE_SYSTEMAGENT_H

#include <intelblocks/systemagent.h>

/* Device 0:0.0 PCI configuration space */

#define EPBAR		0x40
#define DMIBAR		0x68
#define SMRAM		0x88	/* System Management RAM Control */
#define  D_OPEN		(1 << 6)
#define  D_CLS		(1 << 5)
#define  D_LCK		(1 << 4)
#define  G_SMRAME	(1 << 3)
#define  C_BASE_SEG	((0 << 2) | (1 << 1) | (0 << 0))
#define CAPID0_A        0xe4
#define  VTD_DISABLE    (1 << 23)

#define BIOS_RESET_CPL		0x5da8
#define GFXVTBAR		0x5400
#define EDRAMBAR		0x5408
#define VTVC0BAR		0x5410
#define REGBAR			0x5420
#define IPUVTBAR		0x7880
#define TBT0BAR			0x7888
#define TBT1BAR			0x7890
#define TBT2BAR			0x7898
#define TBT3BAR			0x78A0
#define MAX_TBT_PCIE_PORT	4

#define  VTBAR_ENABLED		0x01
#define VTBAR_MASK		0x7ffffff000ull

#define MCH_PKG_POWER_LIMIT_LO	0x59a0
#define MCH_PKG_POWER_LIMIT_HI	0x59a4
#define MCH_DDR_POWER_LIMIT_LO	0x58e0
#define MCH_DDR_POWER_LIMIT_HI	0x58e4

#define IMRBASE			0x6A40
#define IMRLIMIT		0x6A48

static const struct sa_mmio_descriptor soc_vtd_resources[] = {
		{ GFXVTBAR, GFXVT_BASE_ADDRESS, GFXVT_BASE_SIZE, "GFXVTBAR" },
		{ IPUVTBAR, IPUVT_BASE_ADDRESS, IPUVT_BASE_SIZE, "IPUVTBAR" },
		{ TBT0BAR, TBT0_BASE_ADDRESS, TBT0_BASE_SIZE, "TBT0BAR" },
		{ TBT1BAR, TBT1_BASE_ADDRESS, TBT1_BASE_SIZE, "TBT1BAR" },
		{ TBT2BAR, TBT2_BASE_ADDRESS, TBT2_BASE_SIZE, "TBT2BAR" },
		{ TBT3BAR, TBT3_BASE_ADDRESS, TBT3_BASE_SIZE, "TBT3BAR" },
		{ VTVC0BAR, VTVC0_BASE_ADDRESS, VTVC0_BASE_SIZE, "VTVC0BAR" },
};

#define V_P2SB_CFG_IBDF_BUS	0
#define V_P2SB_CFG_IBDF_DEV	30
#define V_P2SB_CFG_IBDF_FUNC	7
#define V_P2SB_CFG_HBDF_BUS	0
#define V_P2SB_CFG_HBDF_DEV	30
#define V_P2SB_CFG_HBDF_FUNC	6

#endif
