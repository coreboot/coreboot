/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __NORTHBRIDGE_INTEL_X4X_H__
#define __NORTHBRIDGE_INTEL_X4X_H__

#include <stdint.h>
#include "memmap.h"

#define BOOT_PATH_NORMAL	0
#define BOOT_PATH_WARM_RESET	1
#define BOOT_PATH_RESUME	2

/*
 * D0:F0
 */
#define HOST_BRIDGE	PCI_DEV(0, 0, 0)

#include "registers/host_bridge.h"

/*
 * D1:F0 PEG
 */
#define PEG_CAP		0xa2
#define SLOTCAP		0xb4
#define PEGLC		0xec
#define D1F0_VCCAP	0x104
#define D1F0_VC0RCTL	0x114

/*
 * Graphics frequencies
 */
#define GCFGC_PCIDEV		PCI_DEV(0, 2, 0)
#define GCFGC_OFFSET		0xf0
#define GCFGC_CR_SHIFT		0
#define GCFGC_CR_MASK		(0xf << GCFGC_CR_SHIFT)
#define GCFGC_CS_SHIFT		8
#define GCFGC_CS_MASK		(0xf << GCFGC_CS_SHIFT)
#define GCFGC_CD_SHIFT		12
#define GCFGC_CD_MASK		(0x1 << GCFGC_CD_SHIFT)
#define GCFGC_UPDATE_SHIFT	5
#define GCFGC_UPDATE		(0x1 << GCFGC_UPDATE_SHIFT)

/*
 * MCHBAR
 */

#include <northbridge/intel/common/fixed_bars.h>

#define MCHBAR8_AND(x,  and) (MCHBAR8(x)  = MCHBAR8(x)  & (and))
#define MCHBAR16_AND(x, and) (MCHBAR16(x) = MCHBAR16(x) & (and))
#define MCHBAR32_AND(x, and) (MCHBAR32(x) = MCHBAR32(x) & (and))
#define MCHBAR8_OR(x,  or) (MCHBAR8(x)  = MCHBAR8(x)  | (or))
#define MCHBAR16_OR(x, or) (MCHBAR16(x) = MCHBAR16(x) | (or))
#define MCHBAR32_OR(x, or) (MCHBAR32(x) = MCHBAR32(x) | (or))
#define MCHBAR8_AND_OR(x,  and, or) (MCHBAR8(x)  = (MCHBAR8(x)  & (and)) | (or))
#define MCHBAR16_AND_OR(x, and, or) (MCHBAR16(x) = (MCHBAR16(x) & (and)) | (or))
#define MCHBAR32_AND_OR(x, and, or) (MCHBAR32(x) = (MCHBAR32(x) & (and)) | (or))

#define CHDECMISC	0x111
#define STACKED_MEM	(1 << 1)

#define C0DRB0		0x200
#define C0DRB1		0x202
#define C0DRB2		0x204
#define C0DRB3		0x206
#define C0DRA01		0x208
#define C0DRA23		0x20a
#define C0CKECTRL	0x260

#define C1DRB0		0x600
#define C1DRB1		0x602
#define C1DRB2		0x604
#define C1DRB3		0x606
#define C1DRA01		0x608
#define C1DRA23		0x60a
#define C1CKECTRL	0x660

#define PMSTS_MCHBAR		0x0f14	/* Self refresh channel status */
#define PMSTS_WARM_RESET	(1 << 8)
#define PMSTS_BOTH_SELFREFRESH	(3 << 0)

#define CLKCFG_MCHBAR		0x0c00
#define CLKCFG_FSBCLK_SHIFT	0
#define CLKCFG_FSBCLK_MASK	(7 << CLKCFG_FSBCLK_SHIFT)
#define CLKCFG_MEMCLK_SHIFT	4
#define CLKCFG_MEMCLK_MASK	(7 << CLKCFG_MEMCLK_SHIFT)
#define CLKCFG_UPDATE		(1 << 12)

#define SSKPD_MCHBAR		0x0c20 /* 64 bit */

/*
 * DMIBAR
 */

#define DMIVCECH	0x000	/* 32bit */
#define DMIPVCCAP1	0x004	/* 32bit */

#define DMIVC0RCAP	0x010	/* 32bit */
#define DMIVC0RCTL	0x014	/* 32bit */
#define DMIVC0RSTS	0x01a	/* 16bit */
#define  VC0NP		(1 << 1)

#define DMIVC1RCAP	0x01c	/* 32bit */
#define DMIVC1RCTL	0x020	/* 32bit */
#define DMIVC1RSTS	0x026	/* 16bit */
#define  VC1NP		(1 << 1)

#define DMIVCPRCAP	0x028	/* 32bit */
#define DMIVCPRCTL	0x02c	/* 32bit */
#define DMIVCPRSTS	0x032	/* 16bit */
#define  VCPNP		(1 << 1)

#define DMIVCMRCAP	0x034	/* 32bit */
#define DMIVCMRCTL	0x038	/* 32bit */
#define DMIVCMRSTS	0x03e	/* 16bit */
#define  VCMNP		(1 << 1)

#define DMIESD		0x044	/* 32bit */

#define DMILE1D		0x050	/* 32bit */
#define DMILE1A		0x058	/* 64bit */
#define DMILE2D		0x060	/* 32bit */
#define DMILE2A		0x068	/* 64bit */

#define DMILCAP		0x084	/* 32bit */
#define DMILCTL		0x088	/* 16bit */
#define DMILSTS		0x08a	/* 16bit */

#define DMIUESTS	0x1c4	/* 32bit */
#define DMICESTS	0x1d0	/* 32bit */

/*
 * EPBAR
 */

#define EPPVCCAP1	0x004	/* 32bit */
#define EPPVCCTL	0x00c	/* 32bit */

#define EPVC0RCAP	0x010	/* 32bit */
#define EPVC0RCTL	0x014	/* 32bit */
#define EPVC0RSTS	0x01a	/* 16bit */

#define EPVC1RCAP	0x01c	/* 32bit */
#define EPVC1RCTL	0x020	/* 32bit */
#define EPVC1RSTS	0x026	/* 16bit */

#define EPVC1MTS	0x028	/* 32bit */
#define EPVC1ITC	0x02c	/* 32bit */

#define EPESD		0x044	/* 32bit */

#define EPLE1D		0x050	/* 32bit */
#define EPLE1A		0x058	/* 64bit */
#define EPLE2D		0x060	/* 32bit */
#define EPLE2A		0x068	/* 64bit */

#define EP_PORTARB(x)	(0x100 + 4 * (x))	/* 256bit */

void x4x_early_init(void);
void x4x_late_init(void);
void mb_get_spd_map(u8 spd_map[4]);
void mb_pre_raminit_setup(int s3_resume);
u32 decode_igd_memory_size(u32 gms);
u32 decode_igd_gtt_size(u32 gsm);
u32 decode_tseg_size(const u32 esmramc);

#include <device/device.h>
struct acpi_rsdp;
unsigned long northbridge_write_acpi_tables(const struct device *device,
		unsigned long start, struct acpi_rsdp *rsdp);

#endif /* __NORTHBRIDGE_INTEL_X4X_H__ */
