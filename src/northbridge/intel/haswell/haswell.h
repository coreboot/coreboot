/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __NORTHBRIDGE_INTEL_HASWELL_HASWELL_H__
#define __NORTHBRIDGE_INTEL_HASWELL_HASWELL_H__

/* Chipset types */
#define HASWELL_MOBILE	0
#define HASWELL_DESKTOP	1
#define HASWELL_SERVER	2

#include "memmap.h"

#include <southbridge/intel/lynxpoint/pch.h>

/* Everything below this line is ignored in the DSDT */
#ifndef __ACPI__

/* Device 0:0.0 PCI configuration space (Host Bridge) */
#define HOST_BRIDGE	PCI_DEV(0, 0, 0)

#include "hostbridge_regs.h"


/* Device 0:2.0 PCI configuration space (Graphics Device) */

#define MSAC		0x62	/* Multi Size Aperture Control */

/*
 * MCHBAR
 */

#define MCHBAR8(x)  (*((volatile u8  *)(DEFAULT_MCHBAR + (x))))
#define MCHBAR16(x) (*((volatile u16 *)(DEFAULT_MCHBAR + (x))))
#define MCHBAR32(x) (*((volatile u32 *)(DEFAULT_MCHBAR + (x))))
#define MCHBAR8_AND(x,  and) (MCHBAR8(x)  = MCHBAR8(x)  & (and))
#define MCHBAR16_AND(x, and) (MCHBAR16(x) = MCHBAR16(x) & (and))
#define MCHBAR32_AND(x, and) (MCHBAR32(x) = MCHBAR32(x) & (and))
#define MCHBAR8_OR(x,  or) (MCHBAR8(x)  = MCHBAR8(x)  | (or))
#define MCHBAR16_OR(x, or) (MCHBAR16(x) = MCHBAR16(x) | (or))
#define MCHBAR32_OR(x, or) (MCHBAR32(x) = MCHBAR32(x) | (or))
#define MCHBAR8_AND_OR(x,  and, or) (MCHBAR8(x)  = (MCHBAR8(x)  & (and)) | (or))
#define MCHBAR16_AND_OR(x, and, or) (MCHBAR16(x) = (MCHBAR16(x) & (and)) | (or))
#define MCHBAR32_AND_OR(x, and, or) (MCHBAR32(x) = (MCHBAR32(x) & (and)) | (or))

/* As there are many registers, define them on a separate file */
#include "mchbar_regs.h"

#define ARCHDIS		0xff0	/* DMA Remap Engine Policy Control */
#define  DMAR_LCKDN	(1 << 31)
#define  SPCAPCTRL	(1 << 25)
#define  L3HIT2PEND_DIS	(1 << 20)
#define  PRSCAPDIS	(1 << 2)
#define  GLBIOTLBINV	(1 << 1)
#define  GLBCTXTINV	(1 << 0)

/*
 * EPBAR - Egress Port Root Complex Register Block
 */

#define EPBAR8(x)  *((volatile u8  *)(DEFAULT_EPBAR + (x)))
#define EPBAR16(x) *((volatile u16 *)(DEFAULT_EPBAR + (x)))
#define EPBAR32(x) *((volatile u32 *)(DEFAULT_EPBAR + (x)))

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

#define DMIBAR8(x)  *((volatile u8  *)(DEFAULT_DMIBAR + (x)))
#define DMIBAR16(x) *((volatile u16 *)(DEFAULT_DMIBAR + (x)))
#define DMIBAR32(x) *((volatile u32 *)(DEFAULT_DMIBAR + (x)))

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

#define DMILCTL2	0x098	/* 16bit */

#define DMICTL1		0x0f0	/* 32bit */
#define DMICTL2		0x0fc	/* 32bit */

#define DMIUESTS	0x1c4	/* 32bit */
#define DMICESTS	0x1d0	/* 32bit */

#define DMICC		0x208	/* 32bit */

#define DMIL0SLAT	0x22c	/* 32bit */
#define DMILLTC		0x238	/* 32bit */

#define DMI_AFE_PM_TMR	0xc28	/* 32bit */

#define DMIDRCCFG	0xeb4	/* 32bit */

#ifndef __ASSEMBLER__

void intel_northbridge_haswell_finalize_smm(void);

void mb_late_romstage_setup(void); /* optional */

void haswell_early_initialization(void);
void haswell_late_initialization(void);
void set_translation_table(int start, int end, u64 base, int inc);
void haswell_unhide_peg(void);

void report_platform_info(void);

int decode_pcie_bar(u32 *const base, u32 *const len);

#include <device/device.h>

struct acpi_rsdp;
unsigned long northbridge_write_acpi_tables(const struct device *device, unsigned long start,
					    struct acpi_rsdp *rsdp);

#endif /* __ASSEMBLER__ */
#endif /* __ACPI__ */
#endif /* __NORTHBRIDGE_INTEL_HASWELL_HASWELL_H__ */
