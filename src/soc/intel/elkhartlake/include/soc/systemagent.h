/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_ELKHARTLAKE_SYSTEMAGENT_H
#define SOC_ELKHARTLAKE_SYSTEMAGENT_H

#include <intelblocks/systemagent.h>

/* Device 0:0.0 PCI configuration space */

#define EPBAR		0x40
#define DMIBAR		0x68
#define CAPID0_A        0xe4
#define  VTD_DISABLE    (1 << 23)

#define BIOS_RESET_CPL		0x5da8
#define GFXVTBAR		0x5400
#define EDRAMBAR		0x5408
#define VTVC0BAR		0x5410
#define REGBAR			0x5420
#define VTBAR_ENABLED		0x01
#define VTBAR_MASK		0x7ffffff000ull

#define MCH_PKG_POWER_LIMIT_LO	0x59a0
#define PKG_PWR_LIM_1_EN	(1 << 15)
#define MCH_PKG_POWER_LIMIT_HI	0x59a4
#define PKG_PWR_LIM_2_EN	(1 << 15)
#define MCH_DDR_POWER_LIMIT_LO	0x58e0
#define MCH_DDR_POWER_LIMIT_HI	0x58e4

#define IMRBASE			0x6A40
#define IMRLIMIT		0x6A48

static const struct sa_mmio_descriptor soc_vtd_resources[] = {
		{ GFXVTBAR, GFXVT_BASE_ADDRESS, GFXVT_BASE_SIZE, "GFXVTBAR" },
		{ VTVC0BAR, VTVC0_BASE_ADDRESS, VTVC0_BASE_SIZE, "VTVC0BAR" },
};

#define V_P2SB_CFG_IBDF_BUS	0
#define V_P2SB_CFG_IBDF_DEV	30
#define V_P2SB_CFG_IBDF_FUNC	7
#define V_P2SB_CFG_HBDF_BUS	0
#define V_P2SB_CFG_HBDF_DEV	30
#define V_P2SB_CFG_HBDF_FUNC	6

#endif
