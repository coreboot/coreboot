/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_SYSTEMAGENT_H_
#define _BROADWELL_SYSTEMAGENT_H_

#include <soc/iomap.h>

#define SA_IGD_OPROM_VENDEV	0x80860406

#define IGD_HASWELL_ULT_GT1	0x0a06
#define IGD_HASWELL_ULT_GT2	0x0a16
#define IGD_HASWELL_ULT_GT3	0x0a26
#define IGD_HASWELL_ULX_GT1	0x0a0e
#define IGD_HASWELL_ULX_GT2	0x0a1e
#define IGD_BROADWELL_U_GT1	0x1606
#define IGD_BROADWELL_U_GT2	0x1616
#define IGD_BROADWELL_U_GT3_15W	0x1626
#define IGD_BROADWELL_U_GT3_28W	0x162b
#define IGD_BROADWELL_Y_GT2	0x161e
#define IGD_BROADWELL_H_GT2	0x1612
#define IGD_BROADWELL_H_GT3	0x1622

#define MCH_BROADWELL_ID_U_Y	0x1604
#define MCH_BROADWELL_REV_D0	0x06
#define MCH_BROADWELL_REV_E0	0x08
#define MCH_BROADWELL_REV_F0	0x09

/* Device 0:0.0 PCI configuration space */

#define EPBAR		0x40
#define MCHBAR		0x48
#define GGC		0x50	/* GMCH Graphics Control */
#define DEVEN		0x54	/* Device Enable */
#define  DEVEN_D7EN	(1 << 14)
#define  DEVEN_D4EN	(1 << 7)
#define  DEVEN_D3EN	(1 << 5)
#define  DEVEN_D2EN	(1 << 4)
#define  DEVEN_D1F0EN	(1 << 3)
#define  DEVEN_D1F1EN	(1 << 2)
#define  DEVEN_D1F2EN	(1 << 1)
#define  DEVEN_D0EN	(1 << 0)
#define DPR		0x5c
#define  DPR_EPM	(1 << 2)
#define  DPR_PRS	(1 << 1)
#define  DPR_SIZE_MASK	0xff0
#define PCIEXBAR	0x60
#define DMIBAR		0x68

#define MESEG_BASE	0x70	/* Management Engine Base. */
#define MESEG_LIMIT	0x78	/* Management Engine Limit. */

#define PAM0		0x80
#define PAM1		0x81
#define PAM2		0x82
#define PAM3		0x83
#define PAM4		0x84
#define PAM5		0x85
#define PAM6		0x86

#define SMRAM		0x88	/* System Management RAM Control */
#define  D_OPEN		(1 << 6)
#define  D_CLS		(1 << 5)
#define  D_LCK		(1 << 4)
#define  G_SMRAME	(1 << 3)
#define  C_BASE_SEG	((0 << 2) | (1 << 1) | (0 << 0))

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
#define  PRSCAPDIS	(1 << 2)

/* MCHBAR */

#define MCHBAR8(x)	*((volatile u8  *)(MCH_BASE_ADDRESS + (x)))
#define MCHBAR16(x)	*((volatile u16 *)(MCH_BASE_ADDRESS + (x)))
#define MCHBAR32(x)	*((volatile u32 *)(MCH_BASE_ADDRESS + (x)))

#define MAD_CHNL		0x5000
#define MAD_DIMM(ch)		(0x5004 + 4 * (ch))

#define MRC_REVISION		0x5034

#define GFXVTBAR		0x5400
#define EDRAMBAR		0x5408
#define VTVC0BAR		0x5410
#define MCH_PAIR		0x5418
#define GDXCBAR			0x5420

#define MCH_DDR_POWER_LIMIT_LO	0x58e0
#define MCH_DDR_POWER_LIMIT_HI	0x58e4

#define MCH_PKG_POWER_LIMIT_LO	0x59a0
#define MCH_PKG_POWER_LIMIT_HI	0x59a4

/* PCODE MMIO communications live in the MCHBAR */
#define BIOS_MAILBOX_DATA	0x5da0

#define BIOS_MAILBOX_INTERFACE	0x5da4
#define  MAILBOX_RUN_BUSY			(1 << 31)
#define  MAILBOX_BIOS_CMD_READ_PCS		1
#define  MAILBOX_BIOS_CMD_WRITE_PCS		2
#define  MAILBOX_BIOS_CMD_READ_CALIBRATION	0x509
#define  MAILBOX_BIOS_CMD_FSM_MEASURE_INTVL	0x909
#define  MAILBOX_BIOS_CMD_READ_PCH_POWER	0xa
#define  MAILBOX_BIOS_CMD_READ_PCH_POWER_EXT	0xb
#define  MAILBOX_BIOS_CMD_READ_C9C10_VOLTAGE	0x26
#define  MAILBOX_BIOS_CMD_WRITE_C9C10_VOLTAGE	0x27

/* Errors are returned back in bits 7:0 */
#define  MAILBOX_BIOS_ERROR_NONE		0
#define  MAILBOX_BIOS_ERROR_INVALID_COMMAND	1
#define  MAILBOX_BIOS_ERROR_TIMEOUT		2
#define  MAILBOX_BIOS_ERROR_ILLEGAL_DATA	3
#define  MAILBOX_BIOS_ERROR_RESERVED		4
#define  MAILBOX_BIOS_ERROR_ILLEGAL_VR_ID	5
#define  MAILBOX_BIOS_ERROR_VR_INTERFACE_LOCKED	6
#define  MAILBOX_BIOS_ERROR_VR_ERROR		7

#define BIOS_RESET_CPL		0x5da8

#define MC_BIOS_DATA		0x5e04

/* System Agent identification */
u8 systemagent_revision(void);

uintptr_t sa_get_tolud_base(void);
uintptr_t sa_get_gsm_base(void);

#endif
