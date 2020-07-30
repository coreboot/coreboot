/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_ICELAKE_SYSTEMAGENT_H
#define SOC_ICELAKE_SYSTEMAGENT_H

#include <intelblocks/systemagent.h>

/* Device 0:0.0 PCI configuration space */

#define EPBAR		0x40
#define DMIBAR		0x68
#define CAPID0_A	0xe4

#define BIOS_RESET_CPL		0x5da8
#define EDRAMBAR		0x5408
#define REGBAR			0x5420

#define MCH_PKG_POWER_LIMIT_LO	0x59a0
#define MCH_PKG_POWER_LIMIT_HI	0x59a4
#define MCH_DDR_POWER_LIMIT_LO	0x58e0
#define MCH_DDR_POWER_LIMIT_HI	0x58e4

#define IMRBASE			0x6A40
#define IMRLIMIT		0x6A48

#endif
