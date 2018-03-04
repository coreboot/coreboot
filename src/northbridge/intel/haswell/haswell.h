/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
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

#ifndef __NORTHBRIDGE_INTEL_HASWELL_HASWELL_H__
#define __NORTHBRIDGE_INTEL_HASWELL_HASWELL_H__

/* Chipset types */
#define HASWELL_MOBILE	0
#define HASWELL_DESKTOP	1
#define HASWELL_SERVER	2

/* Intel Enhanced Debug region */
#define IED_SIZE	CONFIG_IED_REGION_SIZE

/* Northbridge BARs */
#define DEFAULT_PCIEXBAR	CONFIG_MMCONF_BASE_ADDRESS	/* 4 KB per PCIe device */
#define DEFAULT_MCHBAR		0xfed10000	/* 16 KB */
#ifndef __ACPI__
#define DEFAULT_DMIBAR		((u8 *)0xfed18000)	/* 4 KB */
#else
#define DEFAULT_DMIBAR		0xfed18000	/* 4 KB */
#endif
#define DEFAULT_EPBAR		0xfed19000	/* 4 KB */

#define GFXVT_BASE_ADDRESS	0xfed90000ULL
#define GFXVT_BASE_SIZE		0x1000

#define VTVC0_BASE_ADDRESS	0xfed91000ULL
#define VTVC0_BASE_SIZE		0x1000

#include <southbridge/intel/lynxpoint/pch.h>

/* Everything below this line is ignored in the DSDT */
#ifndef __ACPI__

/* Device 0:0.0 PCI configuration space (Host Bridge) */

#define EPBAR		0x40
#define MCHBAR		0x48
#define PCIEXBAR	0x60
#define DMIBAR		0x68

#define GGC		0x50			/* GMCH Graphics Control */

#define DEVEN		0x54			/* Device Enable */
#define  DEVEN_D7EN	(1 << 14)
#define  DEVEN_D4EN	(1 << 7)
#define  DEVEN_D3EN	(1 << 5)
#define  DEVEN_D2EN	(1 << 4)
#define  DEVEN_D1F0EN	(1 << 3)
#define  DEVEN_D1F1EN	(1 << 2)
#define  DEVEN_D1F2EN	(1 << 1)
#define  DEVEN_D0EN	(1 << 0)

#define PAM0		0x80
#define PAM1		0x81
#define PAM2		0x82
#define PAM3		0x83
#define PAM4		0x84
#define PAM5		0x85
#define PAM6		0x86

#define LAC		0x87	/* Legacy Access Control */
#define SMRAM		0x88	/* System Management RAM Control */
#define  D_OPEN		(1 << 6)
#define  D_CLS		(1 << 5)
#define  D_LCK		(1 << 4)
#define  G_SMRAME	(1 << 3)
#define  C_BASE_SEG	((0 << 2) | (1 << 1) | (0 << 0))

#define MESEG_BASE	0x70	/* Management Engine Base. */
#define MESEG_LIMIT	0x78	/* Management Engine Limit. */
#define REMAPBASE	0x90	/* Remap base. */
#define REMAPLIMIT	0x98	/* Remap limit. */
#define TOM		0xa0	/* Top of DRAM in memory controller space. */
#define TOUUD		0xa8	/* Top of Upper Usable DRAM */
#define BDSM		0xb0	/* Base Data Stolen Memory */
#define BGSM		0xb4	/* Base GTT Stolen Memory */
#define TSEG		0xb8	/* TSEG base */
#define TOLUD		0xbc	/* Top of Low Used Memory */

#define SKPAD		0xdc	/* Scratchpad Data */

#define CAPID0_A	0xe4
#define  VTD_DISABLE	(1 << 23)
#define ARCHDIS		0xff0	/* DMA Remap Engine Policy Control */
#define  DMAR_LCKDN	(1 << 31)
#define  SPCAPCTRL	(1 << 25)
#define  L3HIT2PEND_DIS	(1 << 20)
#define  PRSCAPDIS	(1 << 2)
#define  GLBIOTLBINV	(1 << 1)
#define  GLBCTXTINV	(1 << 0)

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
#define GFXVTBAR	0x5400
#define VTVC0BAR	0x5410

/* Some power MSRs are also represented in MCHBAR */
#define MCH_PKG_POWER_LIMIT_LO	0x59a0
#define MCH_PKG_POWER_LIMIT_HI	0x59a4
#define MCH_DDR_POWER_LIMIT_LO	0x58e0
#define MCH_DDR_POWER_LIMIT_HI	0x58e4

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
#include <compiler.h>
static inline void barrier(void) { asm("" ::: "memory"); }

struct ied_header {
	char signature[10];
	u32 size;
	u8 reserved[34];
} __packed;

#define PCI_DEVICE_ID_HSW_MOBILE 0x0c04
#define PCI_DEVICE_ID_HSW_ULT 0x0a04

#ifdef __SMM__
void intel_northbridge_haswell_finalize_smm(void);
#else /* !__SMM__ */
void haswell_early_initialization(int chipset_type);
void haswell_late_initialization(void);
void set_translation_table(int start, int end, u64 base, int inc);

/* debugging functions */
void print_pci_devices(void);
void dump_pci_device(unsigned dev);
void dump_pci_devices(void);
void dump_spd_registers(void);
void dump_mem(unsigned start, unsigned end);
void report_platform_info(void);
#endif /* !__SMM__ */

#if ENV_RAMSTAGE && !defined(__SIMPLE_DEVICE__)
#include <device/device.h>

struct acpi_rsdp;
unsigned long northbridge_write_acpi_tables(device_t device,
		unsigned long start, struct acpi_rsdp *rsdp);
#endif

#endif
#endif
#endif /* __NORTHBRIDGE_INTEL_HASWELL_HASWELL_H__ */
