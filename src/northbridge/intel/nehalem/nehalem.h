/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
 * Copyright (C) 2013 Vladimir Serbinenko
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

#ifndef __NORTHBRIDGE_INTEL_NEHALEM_NEHALEM_H__
#define __NORTHBRIDGE_INTEL_NEHALEM_NEHALEM_H__

#ifndef __ASSEMBLER__

typedef struct {
	unsigned int	CAS;
	unsigned int	tRAS;
	unsigned int	tRP;
	unsigned int	tRCD;
	unsigned int	tRFC;
	unsigned int	tWR;
	unsigned int	tRD;
	unsigned int	tRRD;
	unsigned int	tFAW;
	unsigned int	tWL;
} timings_t;

/* The setup is one DIMM per channel, so there's no need to find a
   common timing setup between multiple chips (but chip and controller
   still need to be coordinated */
typedef struct {
	int		txt_enabled;
	int		cores;
	int		max_ddr2_mhz;
	int		max_ddr3_mt;
	int		max_fsb_mhz;
	int		max_render_mhz;

	int		spd_type;
	timings_t	selected_timings;
} sysinfo_t;

#endif

#define DEFAULT_HECIBAR		((u8 *)0xfed17000)

				/* 4 KB per PCIe device */
#define DEFAULT_PCIEXBAR	CONFIG_MMCONF_BASE_ADDRESS

#define IOMMU_BASE1 0xfed90000
#define IOMMU_BASE2 0xfed91000
#define IOMMU_BASE3 0xfed92000
#define IOMMU_BASE4 0xfed93000

/*
 * D0:F0
 */
#define D0F0_EPBAR_LO 0x40
#define D0F0_EPBAR_HI 0x44
#define D0F0_MCHBAR_LO 0x48
#define D0F0_MCHBAR_HI 0x4c
#define D0F0_GGC 0x52
#define D0F0_DEVEN 0x54
/* Note: Intel's datasheet is broken. Assume the following values are correct */
#define  DEVEN_PEG60	(1 << 13)
#define  DEVEN_IGD	(1 << 3)
#define  DEVEN_PEG10	(1 << 1)
#define  DEVEN_HOST	(1 << 0)
#define D0F0_PCIEXBAR_LO 0x60
#define D0F0_PCIEXBAR_HI 0x64
#define D0F0_DMIBAR_LO 0x68
#define D0F0_DMIBAR_HI 0x6c
#define D0F0_PMBASE 0x78
#define QPD0F1_PAM(x) (0x40+(x)) /* 0-6*/
#define D0F0_REMAPBASE 0x98
#define D0F0_REMAPLIMIT 0x9a
#define D0F0_TOM 0xa0
#define D0F0_TOUUD 0xa2
#define D0F0_IGD_BASE 0xa4
#define D0F0_GTT_BASE 0xa8
#define D0F0_TOLUD 0xb0
#define D0F0_SKPD 0xdc /* Scratchpad Data */

#define D0F0_CAPID0 0xe0

#define TSEG		0xac	/* TSEG base */

/*
 * D1:F0 PEG
 */
#define PEG_CAP 0xa2
#define SLOTCAP 0xb4
#define PEGLC 0xec
#define D1F0_VCCAP 0x104
#define D1F0_VC0RCTL 0x114

/*
 * MCHBAR
 */

#define MCHBAR8(x) *((volatile u8 *)(DEFAULT_MCHBAR + x))
#define MCHBAR16(x) *((volatile u16 *)(DEFAULT_MCHBAR + x))
#define MCHBAR32(x) *((volatile u32 *)(DEFAULT_MCHBAR + x))

/*
 * DMIBAR
 */

#define DMIBAR8(x) *((volatile u8 *)(DEFAULT_DMIBAR + x))
#define DMIBAR16(x) *((volatile u16 *)(DEFAULT_DMIBAR + x))
#define DMIBAR32(x) *((volatile u32 *)(DEFAULT_DMIBAR + x))

#define DMIVC0RCTL 0x14
#define DMIESD  0x44

/*
 * EPBAR
 */

#define EPBAR8(x) *((volatile u8 *)(DEFAULT_EPBAR + x))
#define EPBAR16(x) *((volatile u16 *)(DEFAULT_EPBAR + x))
#define EPBAR32(x) *((volatile u32 *)(DEFAULT_EPBAR + x))

/* Chipset types */
#define NEHALEM_MOBILE	0
#define NEHALEM_DESKTOP	1
#define NEHALEM_SERVER	2

/* Device ID for SandyBridge and IvyBridge */
#define BASE_REV_SNB	0x00
#define BASE_REV_IVB	0x50
#define BASE_REV_MASK	0x50

/* SandyBridge CPU stepping */
#define SNB_STEP_D0	(BASE_REV_SNB + 5) /* Also J0 */
#define SNB_STEP_D1	(BASE_REV_SNB + 6)
#define SNB_STEP_D2	(BASE_REV_SNB + 7) /* Also J1/Q0 */

/* IvyBridge CPU stepping */
#define IVB_STEP_A0	(BASE_REV_IVB + 0)
#define IVB_STEP_B0	(BASE_REV_IVB + 2)
#define IVB_STEP_C0	(BASE_REV_IVB + 4)
#define IVB_STEP_K0	(BASE_REV_IVB + 5)
#define IVB_STEP_D0	(BASE_REV_IVB + 6)

/* Intel Enhanced Debug region must be 4MB */
#define IED_SIZE	0x400000

/* Northbridge BARs */
#define DEFAULT_PCIEXBAR	CONFIG_MMCONF_BASE_ADDRESS	/* 4 KB per PCIe device */
#ifndef __ACPI__
#define DEFAULT_MCHBAR		((u8 *)0xfed10000)	/* 16 KB */
#define DEFAULT_DMIBAR		((u8 *)0xfed18000)	/* 4 KB */
#else
#define DEFAULT_MCHBAR		0xfed10000	/* 16 KB */
#define DEFAULT_DMIBAR		0xfed18000	/* 4 KB */
#endif
#define DEFAULT_EPBAR		0xfed19000	/* 4 KB */
#define DEFAULT_RCBABASE	((u8 *)0xfed1c000)

#define QUICKPATH_BUS 0xff

#include <southbridge/intel/common/rcba.h>
#include <southbridge/intel/ibexpeak/pch.h>

/* Everything below this line is ignored in the DSDT */
#ifndef __ACPI__

/* Device 0:0.0 PCI configuration space (Host Bridge) */

#define EPBAR		0x40
#define MCHBAR		0x48
#define PCIEXBAR	0x60
#define DMIBAR		0x68
#define X60BAR		0x60

#define LAC		0x87	/* Legacy Access Control */
#define QPD0F1_SMRAM		0x4d	/* System Management RAM Control */

#define SKPAD		0xdc	/* Scratchpad Data */

/* Device 0:1.0 PCI configuration space (PCI Express) */

#define BCTRL1		0x3e	/* 16bit */


/* Device 0:2.0 PCI configuration space (Graphics Device) */

#define MSAC		0x62	/* Multi Size Aperture Control */

/*
 * MCHBAR
 */

#define MCHBAR8(x) *((volatile u8 *)(DEFAULT_MCHBAR + x))
#define MCHBAR16(x) *((volatile u16 *)(DEFAULT_MCHBAR + x))
#define MCHBAR32(x) *((volatile u32 *)(DEFAULT_MCHBAR + x))
#define MCHBAR32_OR(x, or) MCHBAR32(x) = (MCHBAR32(x) | (or))

#define BIOS_RESET_CPL	0x5da8	/* 8bit */

/*
 * EPBAR - Egress Port Root Complex Register Block
 */

#define EPBAR8(x) *((volatile u8 *)(DEFAULT_EPBAR + x))
#define EPBAR16(x) *((volatile u16 *)(DEFAULT_EPBAR + x))
#define EPBAR32(x) *((volatile u32 *)(DEFAULT_EPBAR + x))

#define EPPVCCAP1	0x004	/* 32bit */
#define EPPVCCAP2	0x008	/* 32bit */

#define EPVC0RCAP	0x010	/* 32bit */
#define EPVC0RCTL	0x014	/* 32bit */
#define EPVC0RSTS	0x01a	/* 16bit */

#define EPVC1RCAP	0x01c	/* 32bit */
#define EPVC1RCTL	0x020	/* 32bit */
#define EPVC1RSTS	0x026	/* 16bit */

#define EPVC1MTS	0x028	/* 32bit */
#define EPVC1IST	0x038	/* 64bit */

#define EPESD		0x044	/* 32bit */

#define EPLE1D		0x050	/* 32bit */
#define EPLE1A		0x058	/* 64bit */
#define EPLE2D		0x060	/* 32bit */
#define EPLE2A		0x068	/* 64bit */

#define PORTARB		0x100	/* 256bit */

/*
 * DMIBAR
 */

#define DMIBAR8(x) *((volatile u8 *)(DEFAULT_DMIBAR + x))
#define DMIBAR16(x) *((volatile u16 *)(DEFAULT_DMIBAR + x))
#define DMIBAR32(x) *((volatile u32 *)(DEFAULT_DMIBAR + x))

#define DMIVCECH	0x000	/* 32bit */
#define DMIPVCCAP1	0x004	/* 32bit */
#define DMIPVCCAP2	0x008	/* 32bit */

#define DMIPVCCCTL	0x00c	/* 16bit */

#define DMIVC0RCAP	0x010	/* 32bit */
#define DMIVC0RCTL0	0x014	/* 32bit */
#define DMIVC0RSTS	0x01a	/* 16bit */

#define DMIVC1RCAP	0x01c	/* 32bit */
#define DMIVC1RCTL	0x020	/* 32bit */
#define DMIVC1RSTS	0x026	/* 16bit */

#define DMILE1D		0x050	/* 32bit */
#define DMILE1A		0x058	/* 64bit */
#define DMILE2D		0x060	/* 32bit */
#define DMILE2A		0x068	/* 64bit */

#define DMILCAP		0x084	/* 32bit */
#define DMILCTL		0x088	/* 16bit */
#define DMILSTS		0x08a	/* 16bit */

#define DMICTL1		0x0f0	/* 32bit */
#define DMICTL2		0x0fc	/* 32bit */

#define DMICC		0x208	/* 32bit */

#define DMIDRCCFG	0xeb4	/* 32bit */

#ifndef __ASSEMBLER__
static inline void barrier(void) { asm("" ::: "memory"); }

#define PCI_DEVICE_ID_SB 0x0104
#define PCI_DEVICE_ID_IB 0x0154

#ifdef __SMM__
void intel_nehalem_finalize_smm(void);
#else /* !__SMM__ */
int bridge_silicon_revision(void);
void nehalem_early_initialization(int chipset_type);
void nehalem_late_initialization(void);

/* debugging functions */
void print_pci_devices(void);
void dump_pci_device(unsigned dev);
void dump_pci_devices(void);
void dump_spd_registers(void);
void dump_mem(unsigned start, unsigned end);
void report_platform_info(void);
#endif /* !__SMM__ */

#endif
#endif
#endif /* __NORTHBRIDGE_INTEL_NEHALEM_NEHALEM_H__ */
