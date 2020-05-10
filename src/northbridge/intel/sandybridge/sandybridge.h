/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __NORTHBRIDGE_INTEL_SANDYBRIDGE_SANDYBRIDGE_H__
#define __NORTHBRIDGE_INTEL_SANDYBRIDGE_SANDYBRIDGE_H__

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

/* Northbridge BARs */
#ifndef __ACPI__
#define DEFAULT_MCHBAR		((u8 *)0xfed10000)	/* 16 KB */
#define DEFAULT_DMIBAR		((u8 *)0xfed18000)	/* 4 KB */
#else
#define DEFAULT_MCHBAR		0xfed10000	/* 16 KB */
#define DEFAULT_DMIBAR		0xfed18000	/* 4 KB */
#endif
#define DEFAULT_EPBAR		0xfed19000	/* 4 KB */
#define DEFAULT_RCBABASE	((u8 *)0xfed1c000)

#define GFXVT_BASE		0xfed90000ULL
#define VTVC0_BASE		0xfed91000ULL

/* Everything below this line is ignored in the DSDT */
#ifndef __ACPI__
#include <cpu/intel/model_206ax/model_206ax.h>

/* Chipset types */
enum platform_type {
	PLATFORM_MOBILE = 0,
	PLATFORM_DESKTOP_SERVER,
};


/* Device 0:0.0 PCI configuration space (Host Bridge) */
#define HOST_BRIDGE	PCI_DEV(0, 0, 0)

#define EPBAR		0x40
#define MCHBAR		0x48

#define GGC		0x50	/* GMCH Graphics Control */
#define DEVEN		0x54	/* Device Enable */
#define  DEVEN_D7EN	(1 << 14)
#define  DEVEN_PEG60	(1 << 13)
#define  DEVEN_D4EN	(1 <<  7)
#define  DEVEN_IGD	(1 <<  4)
#define  DEVEN_PEG10	(1 <<  3)
#define  DEVEN_PEG11	(1 <<  2)
#define  DEVEN_PEG12	(1 <<  1)
#define  DEVEN_HOST	(1 <<  0)

#define PAVPC		0x58	/* Protected Audio Video Path Control */
#define DPR		0x5c	/* DMA Protected Range */

#define PCIEXBAR	0x60
#define DMIBAR		0x68

#define MESEG_BASE	0x70
#define MESEG_MASK	0x78
#define  MELCK		(1 << 10)	/* ME Range Lock */
#define  ME_STLEN_EN	(1 << 11)	/* ME Stolen Memory Enable */

#define PAM0		0x80
#define PAM1		0x81
#define PAM2		0x82
#define PAM3		0x83
#define PAM4		0x84
#define PAM5		0x85
#define PAM6		0x86

#define LAC		0x87	/* Legacy Access Control */
#define SMRAM		0x88	/* System Management RAM Control */

#define REMAPBASE	0x90
#define REMAPLIMIT	0x98
#define TOM		0xa0
#define TOUUD		0xa8	/* Top of Upper Usable DRAM */
#define BDSM		0xb0	/* Base Data of Stolen Memory */
#define BGSM		0xb4	/* Base GTT Stolen Memory */
#define TSEGMB		0xb8	/* TSEG Memory Base */
#define TOLUD		0xbc	/* Top of Low Used Memory */

#define CAPID0_A	0xe4	/* Capabilities Register A */
#define CAPID0_B	0xe8	/* Capabilities Register B */

#define SKPAD		0xdc	/* Scratchpad Data */

#define DIDOR		0xf3	/* Device ID override, for debug and samples only */


/* Devices 0:1.0, 0:1.1, 0:1.2, 0:6.0 PCI configuration space (PCI Express Graphics) */

#define AFE_PWRON	0xc24	/* PEG Analog Front-End Power-On */


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

/*
 * EPBAR - Egress Port Root Complex Register Block
 */

#define EPBAR8(x)  (*((volatile u8  *)(DEFAULT_EPBAR + (x))))
#define EPBAR16(x) (*((volatile u16 *)(DEFAULT_EPBAR + (x))))
#define EPBAR32(x) (*((volatile u32 *)(DEFAULT_EPBAR + (x))))

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

#define DMIBAR8(x)  (*((volatile u8  *)(DEFAULT_DMIBAR + (x))))
#define DMIBAR16(x) (*((volatile u16 *)(DEFAULT_DMIBAR + (x))))
#define DMIBAR32(x) (*((volatile u32 *)(DEFAULT_DMIBAR + (x))))

#define DMIVCECH	0x000	/* 32bit */
#define DMIPVCCAP1	0x004	/* 32bit */
#define DMIPVCCAP2	0x008	/* 32bit */

#define DMIPVCCCTL	0x00c	/* 16bit */

#define DMIVC0RCAP	0x010	/* 32bit */
#define DMIVC0RCTL	0x014	/* 32bit */
#define DMIVC0RSTS	0x01a	/* 16bit */
#define  VC0NP		0x2

#define DMIVC1RCAP	0x01c	/* 32bit */
#define DMIVC1RCTL	0x020	/* 32bit */
#define DMIVC1RSTS	0x026	/* 16bit */
#define  VC1NP		0x2

#define DMIVCPRCTL	0x02c	/* 32bit */

#define DMIVCPRSTS	0x032	/* 16bit */
#define  VCPNP		0x2

#define DMIVCMRCTL	0x0038	/* 32 bit */
#define DMIVCMRSTS	0x003e	/* 16 bit */
#define  VCMNP		0x2

#define DMILE1D		0x050	/* 32bit */
#define DMILE1A		0x058	/* 64bit */
#define DMILE2D		0x060	/* 32bit */
#define DMILE2A		0x068	/* 64bit */

#define DMILCAP		0x084	/* 32bit */
#define DMILCTL		0x088	/* 16bit */
#define DMILSTS		0x08a	/* 16bit */
#define  TXTRN		(1 << 11)
#define DMICTL1		0x0f0	/* 32bit */
#define DMICTL2		0x0fc	/* 32bit */

#define DMICC		0x208	/* 32bit */

#define DMIDRCCFG	0xeb4	/* 32bit */

#ifndef __ASSEMBLER__

void intel_sandybridge_finalize_smm(void);
int bridge_silicon_revision(void);
void systemagent_early_init(void);
void sandybridge_init_iommu(void);
void sandybridge_late_initialization(void);
void northbridge_romstage_finalize(int s3resume);
void early_init_dmi(void);

/* mainboard_early_init: Optional callback, run after console init but before raminit. */
void mainboard_early_init(int s3resume);
int mainboard_should_reset_usb(int s3resume);
void perform_raminit(int s3resume);
void report_memory_config(void);
enum platform_type get_platform_type(void);

#include <device/device.h>

struct acpi_rsdp;
unsigned long northbridge_write_acpi_tables(const struct device *device, unsigned long start,
					    struct acpi_rsdp *rsdp);

#endif
#endif
#endif /* __NORTHBRIDGE_INTEL_SANDYBRIDGE_SANDYBRIDGE_H__ */
