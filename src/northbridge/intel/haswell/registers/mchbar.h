/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef HASWELL_REGISTERS_MCHBAR_H
#define HASWELL_REGISTERS_MCHBAR_H

/* Memory controller characteristics */
#define NUM_CHANNELS	2
#define NUM_SLOTS	2

/* Indexed register helper macros */
#define _DDRIO_C_R_B(r, ch, rank, byte)	((r) + 0x100 * (ch) + 0x4 * (rank) + 0x200 * (byte))
#define _MCMAIN_C_X(r, ch, x)		((r) + 0x400 * (ch) + 0x4 * (x))
#define _MCMAIN_C(r, ch)		((r) + 0x400 * (ch))

/* Register definitions */

/* DDR DATA per-channel per-bytelane */
#define RX_TRAIN_ch_r_b(ch, rank, byte)		_DDRIO_C_R_B(0x0000, ch, rank, byte)
#define TX_TRAIN_ch_r_b(ch, rank, byte)		_DDRIO_C_R_B(0x0020, ch, rank, byte)

#define DDR_DATA_TRAIN_FEEDBACK(ch, byte)	_DDRIO_C_R_B(0x0054, ch, 0, byte)

#define DQ_CONTROL_2(ch, byte)			_DDRIO_C_R_B(0x0064, ch, 0, byte)
#define DDR_DATA_OFFSET_TRAIN_ch_b(ch, byte)	_DDRIO_C_R_B(0x0070, ch, 0, byte)
#define DQ_CONTROL_0(ch, byte)			_DDRIO_C_R_B(0x0074, ch, 0, byte)

/* DDR CKE per-channel */
#define DDR_CKE_ch_CMD_COMP_OFFSET(ch)		_DDRIO_C_R_B(0x1204, ch, 0, 0)
#define DDR_CKE_ch_CMD_PI_CODING(ch)		_DDRIO_C_R_B(0x1208, ch, 0, 0)

#define DDR_CKE_ch_CTL_CONTROLS(ch)		_DDRIO_C_R_B(0x121c, ch, 0, 0)
#define DDR_CKE_ch_CTL_RANKS_USED(ch)		_DDRIO_C_R_B(0x1220, ch, 0, 0)

/* DDR CTL per-channel */
#define DDR_CTL_ch_CTL_CONTROLS(ch)		_DDRIO_C_R_B(0x1c1c, ch, 0, 0)
#define DDR_CTL_ch_CTL_RANKS_USED(ch)		_DDRIO_C_R_B(0x1c20, ch, 0, 0)

/* DDR CLK per-channel */
#define DDR_CLK_ch_RANKS_USED(ch)		_DDRIO_C_R_B(0x1800, ch, 0, 0)
#define DDR_CLK_ch_COMP_OFFSET(ch)		_DDRIO_C_R_B(0x1808, ch, 0, 0)
#define DDR_CLK_ch_PI_CODING(ch)		_DDRIO_C_R_B(0x180c, ch, 0, 0)
#define DDR_CLK_ch_CONTROLS(ch)			_DDRIO_C_R_B(0x1810, ch, 0, 0)

/* DDR Scrambler */
#define DDR_SCRAMBLE_ch(ch)			(0x2000 + 4 * (ch))
#define DDR_SCRAM_MISC_CONTROL			0x2008

/* DDR DATA per-channel multicast */
#define DDR_DATA_ch_CONTROL_0(ch)		_DDRIO_C_R_B(0x3074, ch, 0, 0)

/* DDR CMDN/CMDS per-channel (writes go to both CMDN and CMDS fubs) */
#define DDR_CMD_ch_COMP_OFFSET(ch)		_DDRIO_C_R_B(0x3204, ch, 0, 0)
#define DDR_CMD_ch_PI_CODING(ch)		_DDRIO_C_R_B(0x3208, ch, 0, 0)
#define DDR_CMD_ch_CONTROLS(ch)			_DDRIO_C_R_B(0x320c, ch, 0, 0)

/* DDR CKE/CTL per-channel (writes go to both CKE and CTL fubs) */
#define DDR_CKE_CTL_ch_CTL_COMP_OFFSET(ch)	_DDRIO_C_R_B(0x3414, ch, 0, 0)
#define DDR_CKE_CTL_ch_CTL_PI_CODING(ch)	_DDRIO_C_R_B(0x3418, ch, 0, 0)

/* DDR DATA broadcast */
#define DDR_DATA_RX_TRAIN_RANK(rank)		_DDRIO_C_R_B(0x3600, 0, rank, 0)
#define DDR_DATA_RX_PER_BIT_RANK(rank)		_DDRIO_C_R_B(0x3610, 0, rank, 0)
#define DDR_DATA_TX_TRAIN_RANK(rank)		_DDRIO_C_R_B(0x3620, 0, rank, 0)
#define DDR_DATA_TX_PER_BIT_RANK(rank)		_DDRIO_C_R_B(0x3630, 0, rank, 0)

#define DDR_DATA_RCOMP_DATA_1			0x3644
#define DDR_DATA_TX_XTALK			0x3648
#define DDR_DATA_RX_OFFSET_VDQ			0x364c
#define DDR_DATA_OFFSET_COMP			0x365c
#define DDR_DATA_CONTROL_1			0x3660

#define DDR_DATA_OFFSET_TRAIN			0x3670
#define DDR_DATA_CONTROL_0			0x3674
#define DDR_DATA_VREF_ADJUST			0x3678

/* DDR CMD broadcast */
#define DDR_CMD_COMP				0x3700

/* DDR CKE/CTL broadcast */
#define DDR_CKE_CTL_COMP			0x3810

/* DDR CLK broadcast */
#define DDR_CLK_COMP				0x3904
#define DDR_CLK_CONTROLS			0x3910
#define DDR_CLK_CB_STATUS			0x3918

/* DDR COMP (global) */
#define DDR_COMP_DATA_COMP_1			0x3a04
#define DDR_COMP_CMD_COMP			0x3a08
#define DDR_COMP_CTL_COMP			0x3a0c
#define DDR_COMP_CLK_COMP			0x3a10
#define DDR_COMP_CTL_0				0x3a14
#define DDR_COMP_CTL_1				0x3a18
#define DDR_COMP_VSSHI				0x3a1c
#define DDR_COMP_OVERRIDE			0x3a20
#define DDR_COMP_VSSHI_CONTROL			0x3a24

/* MCMAIN per-channel */
#define TC_BANK_ch(ch)				_MCMAIN_C(0x4000, ch)
#define TC_BANK_RANK_A_ch(ch)			_MCMAIN_C(0x4004, ch)
#define TC_BANK_RANK_B_ch(ch)			_MCMAIN_C(0x4008, ch)
#define TC_BANK_RANK_C_ch(ch)			_MCMAIN_C(0x400c, ch)
#define COMMAND_RATE_LIMIT_ch(ch)		_MCMAIN_C(0x4010, ch)
#define TC_BANK_RANK_D_ch(ch)			_MCMAIN_C(0x4014, ch)
#define SC_ROUNDT_LAT_ch(ch)			_MCMAIN_C(0x4024, ch)
#define SC_IO_LATENCY_ch(ch)			_MCMAIN_C(0x4028, ch)

#define REUT_ch_PAT_WDB_CL_MUX_CFG(ch)		_MCMAIN_C(0x4040, ch)

#define REUT_ch_PAT_WDB_CL_MUX_WR_x(ch, x)	_MCMAIN_C_X(0x4048, ch, x) /* x in 0 .. 2 */
#define REUT_ch_PAT_WDB_CL_MUX_RD_x(ch, x)	_MCMAIN_C_X(0x4054, ch, x) /* x in 0 .. 2 */

#define REUT_ch_PAT_WDB_CL_MUX_LMN(ch)		_MCMAIN_C(0x4078, ch)

#define REUT_ch_PAT_WDB_INV(ch)			_MCMAIN_C(0x4084, ch)

#define REUT_ch_ERR_CONTROL(ch)			_MCMAIN_C(0x4098, ch)
#define REUT_ch_ERR_ECC_MASK(ch)		_MCMAIN_C(0x409c, ch)

#define SC_WR_ADD_DELAY_ch(ch)			_MCMAIN_C(0x40d0, ch)

#define REUT_ch_ERR_DATA_MASK(ch)		_MCMAIN_C(0x40d8, ch)

#define REUT_ch_ERR_MISC_STATUS(ch)		_MCMAIN_C(0x40e8, ch)

#define REUT_ch_MISC_CKE_CTRL(ch)		_MCMAIN_C(0x4190, ch)
#define REUT_ch_MISC_ODT_CTRL(ch)		_MCMAIN_C(0x4194, ch)
#define REUT_ch_MISC_PAT_CADB_CTRL(ch)		_MCMAIN_C(0x4198, ch)
#define REUT_ch_PAT_CADB_MRS(ch)		_MCMAIN_C(0x419c, ch)
#define REUT_ch_PAT_CADB_MUX_CTRL(ch)		_MCMAIN_C(0x41a0, ch)
#define REUT_ch_PAT_CADB_MUX_x(ch, x)		_MCMAIN_C_X(0x41a4, ch, x) /* x in 0 .. 2 */

#define REUT_ch_PAT_CADB_CL_MUX_LMN(ch)		_MCMAIN_C(0x41b0, ch)
#define REUT_ch_PAT_CADB_WRITE_PTR(ch)		_MCMAIN_C(0x41bc, ch)
#define REUT_ch_PAT_CADB_PROG(ch)		_MCMAIN_C(0x41c0, ch)

#define REUT_ch_WDB_CL_CTRL(ch)			_MCMAIN_C(0x4200, ch)

#define TC_ZQCAL_ch(ch)				_MCMAIN_C(0x4290, ch)
#define TC_RFP_ch(ch)				_MCMAIN_C(0x4294, ch)
#define TC_RFTP_ch(ch)				_MCMAIN_C(0x4298, ch)
#define TC_MR2_SHADOW_ch(ch)			_MCMAIN_C(0x429c, ch)
#define MC_INIT_STATE_ch(ch)			_MCMAIN_C(0x42a0, ch)
#define TC_SRFTP_ch(ch)				_MCMAIN_C(0x42a4, ch)

#define QCLK_ch_LDAT_PDAT(ch)			_MCMAIN_C(0x42d0, ch)
#define QCLK_ch_LDAT_SDAT(ch)			_MCMAIN_C(0x42d4, ch)
#define QCLK_ch_LDAT_DATA_IN_x(ch, x)		_MCMAIN_C_X(0x42dc, ch, x) /* x in 0 .. 1 */

#define REUT_GLOBAL_CTL				0x4800
#define REUT_GLOBAL_ERR				0x4804

#define REUT_ch_SUBSEQ_x_CTL(ch, x)		(0x4808 + 40 * (ch) + 4 * (x))

#define REUT_ch_SEQ_CFG(ch)			(0x48a8 + 8 * (ch))

#define REUT_ch_SEQ_CTL(ch)			(0x48b8 + 4 * (ch))

#define REUT_ch_SEQ_ADDR_START(ch)		(0x48d8 + 8 * (ch))

#define REUT_ch_SEQ_ADDR_WRAP(ch)		(0x48e8 + 8 * (ch))

#define REUT_ch_SEQ_ADDR_CURRENT(ch)		(0x48f8 + 8 * (ch))

#define REUT_ch_SEQ_MISC_CTL(ch)		(0x4908 + 4 * (ch))

#define REUT_ch_SEQ_ADDR_INC_CTL(ch)		(0x4910 + 8 * (ch))

#define REUT_ch_RANK_LOG_TO_PHYS(ch)		(0x4930 + 4 * (ch)) /* 4 bits per rank */

#define HSW_REUT_ch_SEQ_LOOP_COUNT(ch)		(0x4980 + 4 * (ch)) /* *** only on C0 *** */

/* MCMAIN broadcast */
#define MCSCHEDS_CBIT		0x4c20

#define MCSCHEDS_DFT_MISC	0x4c30

#define REUT_ERR_DATA_STATUS	0x4ce0

#define REUT_MISC_CKE_CTRL	0x4d90
#define REUT_MISC_ODT_CTRL	0x4d94

#define MCMNTS_SC_WDBWM		0x4f8c

/* MCDECS */
#define MAD_CHNL		0x5000 /* Address Decoder Channel Configuration */
#define MAD_DIMM(ch)		(0x5004 + (ch) * 4)
#define MAD_ZR			0x5014
#define MC_INIT_STATE_G		0x5030
#define MRC_REVISION		0x5034 /* MRC Revision */

#define RCOMP_TIMER		0x5084

#define MC_LOCK			0x50fc /* Memory Controller Lock register */

#define GFXVTBAR		0x5400 /* Base address for IGD */
#define EDRAMBAR		0x5408 /* Base address for eDRAM */
#define VTVC0BAR		0x5410 /* Base address for PEG, USB, SATA, etc. */
#define INTRDIRCTL		0x5418 /* Interrupt Redirection Control (PAIR) */
#define GDXCBAR			0x5420 /* Generic Debug eXternal Connection */

/* PAVP message register. Bit 0 locks PAVP settings, and bits [31..20] are an offset. */
#define MMIO_PAVP_MSG		0x5500

#define PCU_DDR_PTM_CTL		0x5880

#define PCU_DDR_VOLTAGE		0x58a4

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
#define  MAILBOX_BIOS_CMD_READ_DDR_2X_REFRESH	0x17
#define  MAILBOX_BIOS_CMD_WRITE_DDR_2X_REFRESH	0x18
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

#define MC_BIOS_REQ		0x5e00 /* Memory frequency request register */
#define MC_BIOS_DATA		0x5e04 /* Miscellaneous information for BIOS */
#define SAPMCTL			0x5f00
#define M_COMP			0x5f08

#define HDAUDRID		0x6008
#define UMAGFXCTL		0x6020
#define VDMBDFBARKVM		0x6030
#define VDMBDFBARPAVP		0x6034
#define VTDTRKLCK		0x63fc
#define REQLIM			0x6800
#define DMIVCLIM		0x7000
#define CRDTLCK			0x77fc
#define MCARBLCK		0x7ffc

#endif /* HASWELL_REGISTERS_MCHBAR_H */
