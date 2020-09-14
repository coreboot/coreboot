/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __HASWELL_REGISTERS_MCHBAR_H__
#define __HASWELL_REGISTERS_MCHBAR_H__

/* Register definitions */
#define MAD_CHNL		0x5000 /* Address Decoder Channel Configuration */
#define MAD_DIMM_CH0		0x5004 /* Address Decode Channel 0 */
#define MAD_DIMM_CH1		0x5008 /* Address Decode Channel 1 */
#define MAD_DIMM_CH2		0x500c /* Address Decode Channel 2 (unused on HSW) */
#define MC_INIT_STATE_G		0x5030
#define MRC_REVISION		0x5034 /* MRC Revision */

#define MC_LOCK			0x50fc /* Memory Controlller Lock register */

#define GFXVTBAR		0x5400 /* Base address for IGD */
#define EDRAMBAR		0x5408 /* Base address for eDRAM */
#define VTVC0BAR		0x5410 /* Base address for PEG, USB, SATA, etc. */
#define INTRDIRCTL		0x5418 /* Interrupt Redirection Control (PAIR) */
#define GDXCBAR			0x5420 /* Generic Debug eXternal Connection */

/* PAVP message register. Bit 0 locks PAVP settings, and bits [31..20] are an offset. */
#define MMIO_PAVP_MSG		0x5500

/* Some power MSRs are also represented in MCHBAR */
#define MCH_PKG_POWER_LIMIT_LO	0x59a0
#define MCH_PKG_POWER_LIMIT_HI	0x59a4

#define MCH_DDR_POWER_LIMIT_LO	0x58e0
#define MCH_DDR_POWER_LIMIT_HI	0x58e4

#define SSKPD			0x5d10 /* 64-bit scratchpad register */
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

#endif /* __HASWELL_REGISTERS_MCHBAR_H__ */
