/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __HASWELL_REGISTERS_MCHBAR_H__
#define __HASWELL_REGISTERS_MCHBAR_H__

/* Memory controller characteristics */
#define NUM_CHANNELS	2
#define NUM_SLOTS	2

/* Register definitions */
#define MAD_CHNL		0x5000 /* Address Decoder Channel Configuration */
#define MAD_DIMM(ch)		(0x5004 + (ch) * 4)
#define MC_INIT_STATE_G		0x5030
#define MRC_REVISION		0x5034 /* MRC Revision */

#define MC_LOCK			0x50fc /* Memory Controller Lock register */

#define GFXVTBAR		0x5400 /* Base address for IGD */
#define EDRAMBAR		0x5408 /* Base address for eDRAM */
#define VTVC0BAR		0x5410 /* Base address for PEG, USB, SATA, etc. */
#define INTRDIRCTL		0x5418 /* Interrupt Redirection Control (PAIR) */
#define GDXCBAR			0x5420 /* Generic Debug eXternal Connection */

/* PAVP message register. Bit 0 locks PAVP settings, and bits [31..20] are an offset. */
#define MMIO_PAVP_MSG		0x5500

#define PCU_DDR_PTM_CTL		0x5880

/* Some power MSRs are also represented in MCHBAR */
#define MCH_PKG_POWER_LIMIT_LO	0x59a0
#define MCH_PKG_POWER_LIMIT_HI	0x59a4

#define MCH_DDR_POWER_LIMIT_LO	0x58e0
#define MCH_DDR_POWER_LIMIT_HI	0x58e4

#define SSKPD			0x5d10 /* 64-bit scratchpad register */

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

#define BIOS_RESET_CPL		0x5da8 /* 8-bit */

#define MC_BIOS_DATA		0x5e04 /* Miscellaneous information for BIOS */
#define SAPMCTL			0x5f00

#define HDAUDRID		0x6008
#define UMAGFXCTL		0x6020
#define VDMBDFBARKVM		0x6030
#define VDMBDFBARPAVP		0x6034
#define VTDTRKLCK		0x63fc
#define REQLIM			0x6800
#define DMIVCLIM		0x7000
#define CRDTLCK			0x77fc
#define MCARBLCK		0x7ffc

#endif /* __HASWELL_REGISTERS_MCHBAR_H__ */
