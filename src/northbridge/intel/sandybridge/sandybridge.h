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

#define IOMMU_BASE1		0xfed90000ULL
#define IOMMU_BASE2		0xfed91000ULL

/* Everything below this line is ignored in the DSDT */
#ifndef __ACPI__
#include <cpu/intel/model_206ax/model_206ax.h>

/* Chipset types */
enum platform_type {
	PLATFORM_MOBILE = 0,
	PLATFORM_DESKTOP_SERVER,
};


/* Device 0:0.0 PCI configuration space (Host Bridge) */

#define EPBAR		0x40
#define MCHBAR		0x48
#define PCIEXBAR	0x60
#define DMIBAR		0x68

#define GGC		0x50			/* GMCH Graphics Control */

#define DEVEN		0x54			/* Device Enable */
#define  DEVEN_D7EN	(1 << 14)
#define  DEVEN_PEG60	(1 << 13)
#define  DEVEN_D4EN	(1 << 7)
#define  DEVEN_IGD	(1 << 4)
#define  DEVEN_PEG10	(1 << 3)
#define  DEVEN_PEG11	(1 << 2)
#define  DEVEN_PEG12	(1 << 1)
#define  DEVEN_HOST	(1 << 0)

#define PAVPC		0x58	/* Protected Audio Video Path Control */
#define DPR		0x5c	/* DMA Protected Range */

#define MESEG_BASE	0x70
#define MESEG_MASK	0x78
#define  MELCK		(1 << 10) /* ME Range Lock */
#define  ME_STLEN_EN	(1 << 11) /* ME Stolen Memory Enable */

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


/* Device 0:2.0 PCI configuration space (Graphics Device) */

#define MSAC		0x62	/* Multi Size Aperture Control */

/*
 * MCHBAR
 */

#define MCHBAR8(x) (*((volatile u8 *)(DEFAULT_MCHBAR + (x))))
#define MCHBAR16(x) (*((volatile u16 *)(DEFAULT_MCHBAR + (x))))
#define MCHBAR32(x) (*((volatile u32 *)(DEFAULT_MCHBAR + (x))))
#define MCHBAR32_OR(x, or) (MCHBAR32(x) = (MCHBAR32(x) | (or)))
#define MCHBAR32_AND(x, and) (MCHBAR32(x) = (MCHBAR32(x) & (and)))
#define MCHBAR32_AND_OR(x, and, or) (MCHBAR32(x) = (MCHBAR32(x) & (and)) | (or))

/* Indexed register helper macros */
#define Gz(r, z)	((r) + ((z) <<  8))
#define Ly(r, y)	((r) + ((y) <<  2))
#define Cx(r, x)	((r) + ((x) << 10))
#define CxLy(r, x, y)	((r) + ((x) << 10) + ((y) << 2))
#define GzLy(r, z, y)	((r) + ((z) <<  8) + ((y) << 2))

/* byte lane training register base addresses */
#define LANEBASE_B0	0x0000
#define LANEBASE_B1	0x0200
#define LANEBASE_B2	0x0400
#define LANEBASE_B3	0x0600
#define LANEBASE_ECC	0x0800 /* ECC lane is in the middle of the data lanes */
#define LANEBASE_B4	0x1000
#define LANEBASE_B5	0x1200
#define LANEBASE_B6	0x1400
#define LANEBASE_B7	0x1600

/* byte lane register offsets */
#define GDCRTRAININGRESULT(ch, y)	GzLy(0x0004, ch, y) /* Test results for PI config */
#define GDCRTRAININGRESULT1(ch)		GDCRTRAININGRESULT(ch, 0) /* 0x0004 */
#define GDCRTRAININGRESULT2(ch)		GDCRTRAININGRESULT(ch, 1) /* 0x0008 */
#define GDCRRX(ch, rank)		GzLy(0x10, ch, rank) /* Time setting for lane Rx */
#define GDCRTX(ch, rank)		GzLy(0x20, ch, rank) /* Time setting for lane Tx */

/* Register definitions */
#define GDCRCLKRANKSUSED_ch(ch)		Gz(0x0c00, ch) /* Indicates which rank is populated */
#define GDCRCLKCOMP_ch(ch)		Gz(0x0c04, ch) /* RCOMP result register */
#define GDCRCKPICODE_ch(ch)		Gz(0x0c14, ch) /* PI coding for DDR CLK pins */
#define GDCRCKLOGICDELAY_ch(ch)		Gz(0x0c18, ch) /* Logic delay of 1 QCLK in CLK slice */
#define GDDLLFUSE_ch(ch)		Gz(0x0c20, ch) /* Used for fuse download to the DLLs */
#define GDCRCLKDEBUGMUXCFG_ch(ch)	Gz(0x0c3c, ch) /* Debug MUX control */

#define GDCRCMDDEBUGMUXCFG_Cz_S(ch)	Gz(0x0e3c, ch) /* Debug MUX control */

#define CRCOMPOFST1_ch(ch)		Gz(0x1810, ch) /* DQ, CTL and CLK Offset values */

#define GDCRTRAININGMOD_ch(ch)		Gz(0x3000, ch) /* Data training mode control */
#define GDCRTRAININGRESULT1_ch(ch)	Gz(0x3004, ch) /* Training results according to PI */
#define GDCRTRAININGRESULT2_ch(ch)	Gz(0x3008, ch)

#define GDCRCTLRANKSUSED_ch(ch)		Gz(0x3200, ch) /* Indicates which rank is populated */
#define GDCRCMDCOMP_ch(ch)		Gz(0x3204, ch) /* COMP values register */
#define GDCRCMDCTLCOMP_ch(ch)		Gz(0x3208, ch) /* COMP values register */
#define GDCRCMDPICODING_ch(ch)		Gz(0x320c, ch) /* Command and control PI coding */

#define GDCRTRAININGMOD			0x3400 /* Data training mode control register */
#define GDCRDATACOMP			0x340c /* COMP values register */

#define CRCOMPOFST2			0x3714 /* CMD DRV, SComp and Static Leg controls */

/* MC per-channel registers */
#define TC_DBP_ch(ch)			Cx(0x4000, ch) /* Timings: BIN */
#define TC_RAP_ch(ch)			Cx(0x4004, ch) /* Timings: Regular access */
#define TC_RWP_ch(ch)			Cx(0x4008, ch) /* Timings: Read / Write */
#define TC_OTHP_ch(ch)			Cx(0x400c, ch) /* Timings: Other parameters */
#define SCHED_SECOND_CBIT_ch(ch)	Cx(0x401c, ch) /* More chicken bits */
#define SCHED_CBIT_ch(ch)		Cx(0x4020, ch) /* Chicken bits in scheduler */
#define SC_ROUNDT_LAT_ch(ch)		Cx(0x4024, ch) /* Round-trip latency per rank */
#define SC_IO_LATENCY_ch(ch)		Cx(0x4028, ch) /* IO Latency Configuration */
#define SCRAMBLING_SEED_1_ch(ch)	Cx(0x4034, ch) /* Scrambling seed 1 */
#define SCRAMBLING_SEED_2_LOW_ch(ch)	Cx(0x4038, ch) /* Scrambling seed 2 low */
#define SCRAMBLING_SEED_2_HIGH_ch(ch)	Cx(0x403c, ch) /* Scrambling seed 2 high */

/* IOSAV Bytelane Bit-wise error */
#define IOSAV_By_BW_SERROR_ch(ch, y)	CxLy(0x4040, ch, y)

/* IOSAV Bytelane Bit-wise compare mask */
#define IOSAV_By_BW_MASK_ch(ch, y)	CxLy(0x4080, ch, y)

/*
 * Defines the number of transactions (non-VC1 RD CAS commands) between two priority ticks.
 * Different counters for transactions that are issued on the ring agents (core or GT) and
 * transactions issued in the SA.
 */
#define SC_PR_CNT_CONFIG_ch(ch)	Cx(0x40a8, ch)
#define SC_PCIT_ch(ch)		Cx(0x40ac, ch) /* Page-close idle timer setup - 8 bits */
#define PM_PDWN_CONFIG_ch(ch)	Cx(0x40b0, ch) /* Power-down (CKE-off) operation config */
#define ECC_INJECT_COUNT_ch(ch)	Cx(0x40b4, ch) /* ECC error injection count */
#define ECC_DFT_ch(ch)		Cx(0x40b8, ch) /* ECC DFT features (ECC4ANA, error inject) */
#define SC_WR_ADD_DELAY_ch(ch)	Cx(0x40d0, ch) /* Extra WR delay to overcome WR-flyby issue */

#define IOSAV_By_BW_SERROR_C_ch(ch, y)	CxLy(0x4140, ch, y) /* IOSAV Bytelane Bit-wise error */

/* IOSAV sub-sequence control registers */
#define IOSAV_n_SP_CMD_ADDR_ch(ch, y)	CxLy(0x4200, ch, y) /* Special command address. */
#define IOSAV_n_ADDR_UPD_ch(ch, y)	CxLy(0x4210, ch, y) /* Address update control */
#define IOSAV_n_SP_CMD_CTL_ch(ch, y)	CxLy(0x4220, ch, y) /* Control of command signals */
#define IOSAV_n_SUBSEQ_CTL_ch(ch, y)	CxLy(0x4230, ch, y) /* Sub-sequence controls */
#define IOSAV_n_ADDRESS_LFSR_ch(ch, y)	CxLy(0x4240, ch, y) /* 23-bit LFSR state value */

#define PM_THML_STAT_ch(ch)	Cx(0x4280, ch) /* Thermal status of each rank */
#define IOSAV_SEQ_CTL_ch(ch)	Cx(0x4284, ch) /* IOSAV sequence level control */
#define IOSAV_DATA_CTL_ch(ch)	Cx(0x4288, ch) /* Data control in IOSAV mode */
#define IOSAV_STATUS_ch(ch)	Cx(0x428c, ch) /* State of the IOSAV sequence machine */
#define TC_ZQCAL_ch(ch)		Cx(0x4290, ch) /* ZQCAL control register */
#define TC_RFP_ch(ch)		Cx(0x4294, ch) /* Refresh Parameters */
#define TC_RFTP_ch(ch)		Cx(0x4298, ch) /* Refresh Timing Parameters */
#define TC_MR2_SHADOW_ch(ch)	Cx(0x429c, ch) /* MR2 shadow - copy of DDR configuration */
#define MC_INIT_STATE_ch(ch)	Cx(0x42a0, ch) /* IOSAV mode control */
#define TC_SRFTP_ch(ch)		Cx(0x42a4, ch) /* Self-refresh timing parameters */
#define IOSAV_ERROR_ch(ch)	Cx(0x42ac, ch) /* Data vector count of the first error */
#define IOSAV_DC_MASK_ch(ch)	Cx(0x42b0, ch) /* IOSAV data check masking */

#define IOSAV_By_ERROR_COUNT_ch(ch, y)	CxLy(0x4340, ch, y) /* Per-byte 16-bit error count */
#define IOSAV_G_ERROR_COUNT_ch(ch)	Cx(0x4364, ch) /* Global 16-bit error count */

#define PM_TRML_M_CONFIG_ch(ch)		Cx(0x4380, ch) /* Thermal mode configuration */
#define PM_CMD_PWR_ch(ch)		Cx(0x4384, ch) /* Power contribution of commands */
#define PM_BW_LIMIT_CONFIG_ch(ch)	Cx(0x4388, ch) /* Bandwidth throttling on overtemp */
#define SC_WDBWM_ch(ch)			Cx(0x438c, ch) /* Watermarks and starvation counter */

/* MC Channel Broadcast registers */
#define TC_DBP			0x4c00 /* Timings: BIN */
#define TC_RAP			0x4c04 /* Timings: Regular access */
#define TC_RWP			0x4c08 /* Timings: Read / Write */
#define TC_OTHP			0x4c0c /* Timings: Other parameters */
#define SCHED_SECOND_CBIT	0x4c1c /* More chicken bits */
#define SCHED_CBIT		0x4c20 /* Chicken bits in scheduler */
#define SC_ROUNDT_LAT		0x4c24 /* Round-trip latency per rank */
#define SC_IO_LATENCY		0x4c28 /* IO Latency Configuration */
#define SCRAMBLING_SEED_1	0x4c34 /* Scrambling seed 1 */
#define SCRAMBLING_SEED_2_LOW	0x4c38 /* Scrambling seed 2 low */
#define SCRAMBLING_SEED_2_HIGH	0x4c3c /* Scrambling seed 2 high */

#define IOSAV_By_BW_SERROR(y)	Ly(0x4c40, y) /* IOSAV Bytelane Bit-wise error */
#define IOSAV_By_BW_MASK(y)	Ly(0x4c80, y) /* IOSAV Bytelane Bit-wise compare mask */

/*
 * Defines the number of transactions (non-VC1 RD CAS commands) between two priority ticks.
 * Different counters for transactions that are issued on the ring agents (core or GT) and
 * transactions issued in the SA.
 */
#define SC_PR_CNT_CONFIG	0x4ca8
#define SC_PCIT			0x4cac /* Page-close idle timer setup - 8 bits */
#define PM_PDWN_CONFIG		0x4cb0 /* Power-down (CKE-off) operation config */
#define ECC_INJECT_COUNT	0x4cb4 /* ECC error injection count */
#define ECC_DFT			0x4cb8 /* ECC DFT features (ECC4ANA, error inject) */
#define SC_WR_ADD_DELAY		0x4cd0 /* Extra WR delay to overcome WR-flyby issue */

/* Opportunistic reads configuration during write-major-mode (WMM) */
#define WMM_READ_CONFIG		0x4cd4 /** WARNING: Only exists on IVB! */

#define IOSAV_By_BW_SERROR_C(y)	Ly(0x4d40, y) /* IOSAV Bytelane Bit-wise error */

#define IOSAV_n_SP_CMD_ADDR(n)	Ly(0x4e00, n) /* Sub-sequence special command address */
#define IOSAV_n_ADDR_UPD(n)	Ly(0x4e10, n) /* Address update after command execution */
#define IOSAV_n_SP_CMD_CTL(n)	Ly(0x4e20, n) /* Command signals in sub-sequence command */
#define IOSAV_n_SUBSEQ_CTL(n)	Ly(0x4e30, n) /* Sub-sequence command parameter control */
#define IOSAV_n_ADDRESS_LFSR(n)	Ly(0x4e40, n) /* 23-bit LFSR value of the sequence */

#define PM_THML_STAT		0x4e80 /* Thermal status of each rank */
#define IOSAV_SEQ_CTL		0x4e84 /* IOSAV sequence level control */
#define IOSAV_DATA_CTL		0x4e88 /* Data control in IOSAV mode */
#define IOSAV_STATUS		0x4e8c /* State of the IOSAV sequence machine */
#define TC_ZQCAL		0x4e90 /* ZQCAL control register */
#define TC_RFP			0x4e94 /* Refresh Parameters */
#define TC_RFTP			0x4e98 /* Refresh Timing Parameters */
#define TC_MR2_SHADOW		0x4e9c /* MR2 shadow - copy of DDR configuration */
#define MC_INIT_STATE		0x4ea0 /* IOSAV mode control */
#define TC_SRFTP		0x4ea4 /* Self-refresh timing parameters */

/*
 * Auxiliary register in mcmnts synthesis FUB (Functional Unit Block). Additionally, this
 * register is also used to enable IOSAV_n_SP_CMD_ADDR optimization on Ivy Bridge.
 */
#define MCMNTS_SPARE		0x4ea8 /** WARNING: Reserved, use only on IVB! */

#define IOSAV_ERROR		0x4eac /* Data vector count of the first error */
#define IOSAV_DC_MASK		0x4eb0 /* IOSAV data check masking */

#define IOSAV_By_ERROR_COUNT(y)	Ly(0x4f40, y) /* Per-byte 16-bit error counter */
#define IOSAV_G_ERROR_COUNT	0x4f64 /* Global 16-bit error counter */

#define PM_TRML_M_CONFIG	0x4f80 /* Thermal mode configuration */
#define PM_CMD_PWR		0x4f84 /* Power contribution of commands */
#define PM_BW_LIMIT_CONFIG	0x4f88 /* Bandwidth throttling on overtemperature */
#define SC_WDBWM		0x4f8c /* Watermarks and starvation counter config */

#define MAD_CHNL		0x5000 /* Address Decoder Channel Configuration */
#define MAD_DIMM_CH0		0x5004 /* Address Decode Channel 0 */
#define MAD_DIMM_CH1		0x5008 /* Address Decode Channel 1 */
#define MAD_DIMM_CH2		0x500c /* Address Decode Channel 2 (unused on SNB) */
#define MAD_ZR			0x5014 /* Address Decode Zones */
#define MCDECS_SPARE		0x5018 /* Spare register in mcdecs synthesis FUB */
#define MCDECS_CBIT		0x501c /* Chicken bits in mcdecs synthesis FUB */

#define CHANNEL_HASH		0x5024 /** WARNING: Only exists on IVB! */

#define MC_INIT_STATE_G		0x5030 /* High-level behavior in IOSAV mode */
#define MRC_REVISION		0x5034 /* MRC Revision */
#define PM_DLL_CONFIG		0x5064 /* Memory Controller I/O DLL config */
#define RCOMP_TIMER		0x5084 /* RCOMP evaluation timer register */

#define MC_LOCK			0x50fc /* Memory Controlller Lock register */

#define VTD1_BASE		0x5400 /* Base address for IGD */
#define VTD2_BASE		0x5410 /* Base address for PEG, USB, SATA, etc. */
#define PAIR_CTL		0x5418 /* Power Aware Interrupt Routing Control */

/* PAVP control register, undocumented. Different from PAVPC on PCI config space. */
#define MMIO_PAVP_CTL		0x5500 /* Bit 0 locks PAVP settings */

#define MEM_TRML_ESTIMATION_CONFIG	0x5880
#define MEM_TRML_THRESHOLDS_CONFIG	0x5888
#define MEM_TRML_INTERRUPT		0x58a8

#define MC_TURBO_PL1		0x59a0 /* Turbo Power Limit 1 parameters */
#define MC_TURBO_PL2		0x59a4 /* Turbo Power Limit 2 parameters */

#define SSKPD_OK		0x5d10 /* 64-bit scratchpad register */
#define SSKPD			0x5d14 /* 16bit (scratchpad) */
#define BIOS_RESET_CPL		0x5da8 /* 8bit */

/* PCODE will sample SAPM-related registers at the end of Phase 4. */
#define MC_BIOS_REQ		0x5e00 /* Memory frequency request register */
#define MC_BIOS_DATA		0x5e04 /* Miscellaneous information for BIOS */
#define SAPMCTL			0x5f00 /* Bit 3 enables DDR EPG (C7i) on IVB */
#define M_COMP			0x5f08 /* Memory COMP control */
#define SAPMTIMERS		0x5f10 /* SAPM timers in 10ns (100 MHz) units */

/* WARNING: Only applies to Sandy Bridge! */
#define BANDTIMERS_SNB		0x5f18 /* MPLL and PPLL time to do self-banding */

/** WARNING: Only applies to Ivy Bridge! */
#define SAPMTIMERS2_IVB		0x5f18 /** Extra latency for DDRIO EPG exit (C7i) */
#define BANDTIMERS_IVB		0x5f20 /** MPLL and PPLL time to do self-banding */

/*
 * EPBAR - Egress Port Root Complex Register Block
 */

#define EPBAR8(x) (*((volatile u8 *)(DEFAULT_EPBAR + (x))))
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

#define DMIBAR8(x) (*((volatile u8 *)(DEFAULT_DMIBAR + (x))))
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

/* mainboard_early_init: Optional mainboard callback run after console init
   but before raminit. */
void mainboard_early_init(int s3resume);
int mainboard_should_reset_usb(int s3resume);
void perform_raminit(int s3resume);
enum platform_type get_platform_type(void);

#include <device/device.h>

struct acpi_rsdp;
unsigned long northbridge_write_acpi_tables(struct device *device, unsigned long start, struct acpi_rsdp *rsdp);

#endif
#endif
#endif /* __NORTHBRIDGE_INTEL_SANDYBRIDGE_SANDYBRIDGE_H__ */
