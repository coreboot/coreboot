/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __NORTHBRIDGE_INTEL_IRONLAKE_IRONLAKE_H__
#define __NORTHBRIDGE_INTEL_IRONLAKE_IRONLAKE_H__

#define DEFAULT_HECIBAR		((u8 *)0xfed17000)

/*
 * D1:F0 PEG
 */
#define PEG_CAP		0xa2
#define SLOTCAP		0xb4
#define PEGLC		0xec
#define D1F0_VCCAP	0x104
#define D1F0_VC0RCTL	0x114

/* Chipset types */
#define IRONLAKE_MOBILE		0
#define IRONLAKE_DESKTOP	1
#define IRONLAKE_SERVER		2

#include "memmap.h"

#define QUICKPATH_BUS (CONFIG_MMCONF_BUS_NUMBER - 1)

#include <southbridge/intel/ibexpeak/pch.h>

/* Everything below this line is ignored in the DSDT */
#ifndef __ACPI__

/* Device 0:0.0 PCI configuration space (Host Bridge) */

#include "registers/host_bridge.h"

/*
 * Generic Non-Core Registers
 */
#define QPI_NON_CORE		PCI_DEV(QUICKPATH_BUS, 0, 0)

#define MAX_RTIDS		0x60
#define DESIRED_CORES		0x80
#define MIRROR_PORT_CTL		0xd0

/*
 * SAD - System Address Decoder
 */
#define QPI_SAD			PCI_DEV(QUICKPATH_BUS, 0, 1)

#define QPD0F1_PAM(x)		(0x40 + (x)) /* 0-6 */
#define QPD0F1_SMRAM		0x4d	/* System Management RAM Control */

#define SAD_PCIEXBAR		0x50

#define SAD_DRAM_RULE(x)	(0x80 + 4 * (x)) /* 0-7 */
#define SAD_INTERLEAVE_LIST(x)	(0xc0 + 4 * (x)) /* 0-7 */

/*
 * QPI Link 0
 */
#define QPI_LINK_0		PCI_DEV(QUICKPATH_BUS, 2, 0)

#define QPI_QPILCP		0x40 /* QPI Link Capability */
#define QPI_QPILCL		0x48 /* QPI Link Control */
#define QPI_QPILS		0x50 /* QPI Link Status */
#define QPI_DEF_RMT_VN_CREDITS	0x58 /* Default Available Remote Credits */

/*
 * QPI Physical Layer 0
 */
#define QPI_PHY_0		PCI_DEV(QUICKPATH_BUS, 2, 1)

#define QPI_PLL_STATUS		0x50
#define QPI_PLL_RATIO		0x54
#define QPI_PHY_CAPABILITY	0x68 /* QPI Phys. Layer Capability */
#define QPI_PHY_CONTROL		0x6c /* QPI Phys. Layer Control */
#define QPI_PHY_INIT_STATUS	0x80 /* QPI Phys. Layer Initialization Status */
#define QPI_PHY_PRIM_TIMEOUT	0x94 /* QPI Phys. Layer Primary Timeout Value */
#define QPI_PHY_PWR_MGMT	0xd0 /* QPI Phys. Layer Power Management */
#define QPI_PHY_EP_SELECT	0xe0 /* QPI Phys. Layer Electrical Parameter Select */
#define QPI_PHY_EP_MCTR		0xf4 /* QPI Phys. Layer Electrical Parameter Misc. Control */


/* Device 0:2.0 PCI configuration space (Graphics Device) */

#define MSAC		0x62	/* Multi Size Aperture Control */

/*
 * MCHBAR
 */

#define MCHBAR8(x)			(*((volatile u8  *)((u8 *)DEFAULT_MCHBAR + (x))))
#define MCHBAR16(x)			(*((volatile u16 *)((u8 *)DEFAULT_MCHBAR + (x))))
#define MCHBAR32(x)			(*((volatile u32 *)((u8 *)DEFAULT_MCHBAR + (x))))
#define MCHBAR8_AND(x,  and)		(MCHBAR8(x)  = MCHBAR8(x)  & (and))
#define MCHBAR16_AND(x, and)		(MCHBAR16(x) = MCHBAR16(x) & (and))
#define MCHBAR32_AND(x, and)		(MCHBAR32(x) = MCHBAR32(x) & (and))
#define MCHBAR8_OR(x,  or)		(MCHBAR8(x)  = MCHBAR8(x)  | (or))
#define MCHBAR16_OR(x, or)		(MCHBAR16(x) = MCHBAR16(x) | (or))
#define MCHBAR32_OR(x, or)		(MCHBAR32(x) = MCHBAR32(x) | (or))
#define MCHBAR8_AND_OR(x,  and, or)	(MCHBAR8(x)  = (MCHBAR8(x)  & (and)) | (or))
#define MCHBAR16_AND_OR(x, and, or)	(MCHBAR16(x) = (MCHBAR16(x) & (and)) | (or))
#define MCHBAR32_AND_OR(x, and, or)	(MCHBAR32(x) = (MCHBAR32(x) & (and)) | (or))
/*
 * EPBAR - Egress Port Root Complex Register Block
 */

#define EPBAR8(x)	(*((volatile u8  *)((u8 *)DEFAULT_EPBAR + (x))))
#define EPBAR16(x)	(*((volatile u16 *)((u8 *)DEFAULT_EPBAR + (x))))
#define EPBAR32(x)	(*((volatile u32 *)((u8 *)DEFAULT_EPBAR + (x))))

#include "registers/epbar.h"

/*
 * DMIBAR
 */

#define DMIBAR8(x)	(*((volatile u8  *)((u8 *)DEFAULT_DMIBAR + (x))))
#define DMIBAR16(x)	(*((volatile u16 *)((u8 *)DEFAULT_DMIBAR + (x))))
#define DMIBAR32(x)	(*((volatile u32 *)((u8 *)DEFAULT_DMIBAR + (x))))

#include "registers/dmibar.h"

#ifndef __ASSEMBLER__

void intel_ironlake_finalize_smm(void);

int bridge_silicon_revision(void);
void ironlake_early_initialization(int chipset_type);
void ironlake_late_initialization(void);
void mainboard_pre_raminit(void);
void mainboard_get_spd_map(u8 *spd_addrmap);

#endif
#endif
#endif /* __NORTHBRIDGE_INTEL_IRONLAKE_IRONLAKE_H__ */
