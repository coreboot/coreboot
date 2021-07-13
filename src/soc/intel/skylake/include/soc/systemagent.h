/* SPDX-License-Identifier: GPL-2.0-only */

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

bool soc_vtd_enabled(void);

static const struct sa_mmio_descriptor soc_gfxvt_mmio_descriptor = {
	GFXVTBAR,
	GFXVT_BASE_ADDRESS,
	GFXVT_BASE_SIZE,
	"GFXVTBAR"
};

static const struct sa_mmio_descriptor soc_vtvc0_mmio_descriptor = {
	VTVC0BAR,
	VTVC0_BASE_ADDRESS,
	VTVC0_BASE_SIZE,
	"VTVC0BAR"
};

/* Hardcoded default values for PCI Bus:Dev.Fun for IOAPIC and HPET */
#define V_P2SB_IBDF_BUS	250
#define V_P2SB_IBDF_DEV	31
#define V_P2SB_IBDF_FUN	0
#define V_DEFAULT_IBDF	((V_P2SB_IBDF_BUS << 8) | PCI_DEVFN(V_P2SB_IBDF_DEV, V_P2SB_IBDF_FUN))

#define V_P2SB_HBDF_BUS	250
#define V_P2SB_HBDF_DEV	15
#define V_P2SB_HBDF_FUN	0
#define V_DEFAULT_HBDF	((V_P2SB_HBDF_BUS << 8) | PCI_DEVFN(V_P2SB_HBDF_DEV, V_P2SB_HBDF_FUN))

#endif
