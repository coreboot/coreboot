/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SANDYBRIDGE_MCHBAR_REGS_H__
#define __SANDYBRIDGE_MCHBAR_REGS_H__

/*
 * ### IOSAV command queue notes ###
 *
 * Intel provides a command queue of depth four.
 * Every command is configured by using multiple MCHBAR registers.
 * On executing the command queue, you have to specify its depth (number of commands).
 *
 * The macros for these registers can take some integer parameters, within these bounds:
 *   channel:   [0..1]
 *   index:     [0..3]
 *   lane:      [0..8]
 *
 * Note that these ranges are 'closed': both endpoints are included.
 *
 *
 *
 * ### Register description ###
 *
 * IOSAV_n_SP_CMD_ADDR_ch(channel, index)
 *   Sub-sequence command addresses. Controls the address, bank address and slotrank signals.
 *
 *   Bitfields:
 *   [0..15]    Row / Column Address.
 *   [16..18]   The result of (10 + [16..18]) is the number of valid row bits.
 *                  Note: Value 1 is not implemented. Not that it really matters, though.
 *                        Value 7 is reserved, as the hardware does not support it.
 *   [20..22]   Bank Address.
 *   [24..25]   Rank select. Let's call it "ranksel", as it is mentioned later.
 *
 * IOSAV_n_ADDR_UPDATE_ch(channel, index)
 *   How the address shall be updated after executing the sub-sequence command.
 *
 *   Bitfields:
 *   [0]        Increment CAS/RAS by 1.
 *   [1]        Increment CAS/RAS by 8.
 *   [2]        Increment bank select by 1.
 *   [3..4]     Increment rank select by 1, 2 or 3.
 *   [5..9]     Known as "addr_wrap". Address bits will wrap around the [addr_wrap..0] range.
 *   [10..11]   LFSR update:
 *                  00: Do not use the LFSR function.
 *                  01: Undefined, treat as Reserved.
 *                  10: Apply LFSR on the [addr_wrap..0] bit range.
 *                  11: Apply LFSR on the [addr_wrap..3] bit range.
 *
 *   [12..15]   Update rate. The number of command runs between address updates. For example:
 *                  0: Update every command run.
 *                  1: Update every second command run. That is, half of the command rate.
 *                  N: Update after N command runs without updates.
 *
 *   [16..17]   LFSR behavior on the deselect cycles (when no sub-seq command is issued):
 *                  0: No change w.r.t. the last issued command.
 *                  1: LFSR XORs with address & command (excluding CS), but does not update.
 *                  2: LFSR XORs with address & command (excluding CS), and updates.
 *
 * IOSAV_n_SP_CMD_CTRL_ch(channel, index)
 *   Special command control register. Controls the DRAM command signals.
 *
 *   Bitfields:
 *   [0]        !RAS signal.
 *   [1]        !CAS signal.
 *   [2]        !WE  signal.
 *   [4..7]     CKE, per rank and channel.
 *   [8..11]    ODT, per rank and channel.
 *   [12..15]   Chip select, per rank and channel. It works as follows:
 *
 *          entity CS_BLOCK is
 *              port (
 *                  MODE    : in  std_logic;                -- Mode select at [16]
 *                  RANKSEL : in  std_logic_vector(0 to 3); -- Decoded "ranksel" value
 *                  CS_CTL  : in  std_logic_vector(0 to 3); -- Chip select control at [12..15]
 *                  CS_Q    : out std_logic_vector(0 to 3)  -- CS signals
 *              );
 *          end entity CS_BLOCK;
 *
 *          architecture RTL of CS_BLOCK is
 *          begin
 *              if MODE = '1' then
 *                  CS_Q <= not RANKSEL and CS_CTL;
 *              else
 *                  CS_Q <= CS_CTL;
 *              end if;
 *          end architecture RTL;
 *
 *   [16]       Chip Select mode control.
 *   [17]       Auto Precharge. Only valid when using 10 row bits!
 *
 * IOSAV_n_SUBSEQ_CTRL_ch(channel, index)
 *   Sub-sequence parameters. Controls repetititons, delays and data orientation.
 *
 *   Bitfields:
 *   [0..8]     Number of repetitions of the sub-sequence command.
 *   [10..14]   Gap, number of clock-cycles to wait before sending the next command.
 *   [16..24]   Number of clock-cycles to idle between sub-sequence commands.
 *   [26..27]   The direction of the data.
 *                  00: None, does not handle data
 *                  01: Read
 *                  10: Write
 *                  11: Read & Write
 *
 * IOSAV_n_ADDRESS_LFSR_ch(channel, index)
 *   23-bit LFSR state register. It is written into the LFSR when the sub-sequence is loaded,
 *   and then read back from the LFSR when the sub-sequence is done.
 *
 *   Bitfields:
 *   [0..22]    LFSR state.
 *
 * IOSAV_SEQ_CTL_ch(channel)
 *   Control the sequence level in IOSAV: number of sub-sequences, iterations, maintenance...
 *
 *   Bitfields:
 *   [0..7]     Number of full sequence executions. When this field becomes non-zero, then the
 *              sequence starts running immediately. This value is decremented after completing
 *              a full sequence iteration. When it is zero, the sequence is done. No decrement
 *              is done if this field is set to 0xff. This is the "infinite repeat" mode, and
 *              it is manually aborted by clearing this field.
 *
 *   [8..16]    Number of wait cycles after each sequence iteration. This wait's purpose is to
 *              allow performing maintenance in infinite loops. When non-zero, RCOMP, refresh
 *              and ZQXS operations can take place.
 *
 *   [17]       Stop-on-error mode: Whether to stop sequence execution when an error occurs.
 *   [18..19]   Number of sub-sequences. The programmed value is the index of the last sub-seq.
 *   [20]       If set, keep refresh disabled until the next sequence execution.
 *                  DANGER: Refresh must be re-enabled within the (9 * tREFI) period!
 *
 *   [22]       If set, sequence execution will not prevent refresh. This cannot be set when
 *              bit [20] is also set, or was set on the previous sequence. This bit exists so
 *              that the sequence machine can be used as a timer without affecting the memory.
 *
 *   [23]       If set, a output pin is asserted on the first detected error. This output can
 *              be used as a trigger for an oscilloscope or a logic analyzer, which is handy.
 *
 * IOSAV_DATA_CTL_ch(channel)
 *   Data-related controls in IOSAV mode.
 *
 *   Bitfields:
 *   [0..7]     WDB (Write Data Buffer) pattern length: [0..7] = (length / 8) - 1;
 *   [8..15]    WDB read pointer. Points at the data used for IOSAV write transactions.
 *   [16..23]   Comparison pointer. Used to compare data from IOSAV read transactions.
 *   [24]       If set, increment pointers only when micro-breakpoint is active.
 *
 * IOSAV_STATUS_ch(channel)
 *   State of the IOSAV sequence machine. Should be polled after sending an IOSAV sequence.
 *
 *   Bitfields:
 *   [0]        IDLE:  IOSAV is sleeping.
 *   [1]        BUSY:  IOSAV is running a sequence.
 *   [2]        DONE:  IOSAV has completed a sequence.
 *   [3]        ERROR: IOSAV detected an error and stopped on it, when using Stop-on-error.
 *   [4]        PANIC: The refresh machine issued a Panic Refresh, and IOSAV was aborted.
 *   [5]        RCOMP: RComp failure. Unused, consider Reserved.
 *   [6]        Cleared with a new sequence, and set when done and refresh counter is drained.
 *
 */

/* Temporary IOSAV register macros to verifiably split bitfields */
#define SUBSEQ_CTRL(reps, gap, post, dir)	(((reps) <<  0) | \
						 ((gap)  << 10) | \
						 ((post) << 16) | \
						 ((dir)  << 26))

#define SSQ_NA		0 /* Non-data */
#define SSQ_RD		1 /* Read */
#define SSQ_WR		2 /* Write */
#define SSQ_RW		3 /* Read and write */

#define SP_CMD_ADDR(addr, rowbits, bank, rank)	(((addr)    <<  0) | \
						 ((rowbits) << 16) | \
						 ((bank)    << 20) | \
						 ((rank)    << 24))

#define ADDR_UPDATE(addr_1, addr_8, bank, rank, wrap, lfsr, rate, xors)	(((addr_1) <<  0) | \
									 ((addr_8) <<  1) | \
									 ((bank)   <<  2) | \
									 ((rank)   <<  3) | \
									 ((wrap)   <<  5) | \
									 ((lfsr)   << 10) | \
									 ((rate)   << 12) | \
									 ((xors)   << 16))

/* Marker macro for IOSAV_n_ADDR_UPDATE */
#define ADDR_UPDATE_NONE	0

/* Only programming the wraparound without any triggers is suspicious */
#define ADDR_UPDATE_WRAP(wrap)	((wrap) << 5)

#define IOSAV_SUBSEQUENCE(ch, n, sp_cmd_ctrl, reps, gap, post, dir, addr, rowbits, bank, rank, addr_update) \
	do { \
		MCHBAR32(IOSAV_n_SP_CMD_CTRL_ch(ch, n)) = sp_cmd_ctrl; \
		MCHBAR32(IOSAV_n_SUBSEQ_CTRL_ch(ch, n)) = SUBSEQ_CTRL(reps, gap, post, dir); \
		MCHBAR32(IOSAV_n_SP_CMD_ADDR_ch(ch, n)) = SP_CMD_ADDR(addr, rowbits, bank, rank); \
		MCHBAR32(IOSAV_n_ADDR_UPDATE_ch(ch, n)) = addr_update; \
	} while (0)

/* Indexed register helper macros */
#define Gz(r, z)	((r) + ((z) <<  8))
#define Ly(r, y)	((r) + ((y) <<  2))
#define Cx(r, x)	((r) + ((x) << 10))
#define CxLy(r, x, y)	((r) + ((x) << 10) + ((y) << 2))
#define GzLy(r, z, y)	((r) + ((z) <<  8) + ((y) << 2))

/* Byte lane training register base addresses */
#define LANEBASE_B0	0x0000
#define LANEBASE_B1	0x0200
#define LANEBASE_B2	0x0400
#define LANEBASE_B3	0x0600
#define LANEBASE_ECC	0x0800 /* ECC lane is in the middle of the data lanes */
#define LANEBASE_B4	0x1000
#define LANEBASE_B5	0x1200
#define LANEBASE_B6	0x1400
#define LANEBASE_B7	0x1600

/* Byte lane register offsets */
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

/*
 * The register bank that would correspond to Channel 3 are actually "broadcast" registers.
 * They can be used to write values to all channels. Use this macro instead of a literal '3'.
 */
#define BROADCAST_CH	3

/* MC per-channel registers */
#define TC_DBP_ch(ch)			Cx(0x4000, ch) /* Timings: BIN */
#define TC_RAP_ch(ch)			Cx(0x4004, ch) /* Timings: Regular access */
#define TC_RWP_ch(ch)			Cx(0x4008, ch) /* Timings: Read / Write */
#define TC_OTHP_ch(ch)			Cx(0x400c, ch) /* Timings: Other parameters */

/** WARNING: Only applies to Ivy Bridge! */
#define TC_DTP_ch(ch)			Cx(0x4014, ch) /** Timings: Debug parameters */

#define SCHED_SECOND_CBIT_ch(ch)	Cx(0x401c, ch) /* More chicken bits */
#define SCHED_CBIT_ch(ch)		Cx(0x4020, ch) /* Chicken bits in scheduler */
#define SC_ROUNDT_LAT_ch(ch)		Cx(0x4024, ch) /* Round-trip latency per rank */
#define SC_IO_LATENCY_ch(ch)		Cx(0x4028, ch) /* IO Latency Configuration */
#define SCRAMBLING_SEED_1_ch(ch)	Cx(0x4034, ch) /* Scrambling seed 1 */
#define SCRAMBLING_SEED_2_LO_ch(ch)	Cx(0x4038, ch) /* Scrambling seed 2 low */
#define SCRAMBLING_SEED_2_HI_ch(ch)	Cx(0x403c, ch) /* Scrambling seed 2 high */

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
#define IOSAV_n_ADDR_UPDATE_ch(ch, y)	CxLy(0x4210, ch, y) /* Address update control */
#define IOSAV_n_SP_CMD_CTRL_ch(ch, y)	CxLy(0x4220, ch, y) /* Control of command signals */
#define IOSAV_n_SUBSEQ_CTRL_ch(ch, y)	CxLy(0x4230, ch, y) /* Sub-sequence controls */
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

/** WARNING: Only applies to Ivy Bridge! */
#define IOSAV_BYTE_SERROR_ch(ch)	Cx(0x4368, ch) /** Byte-Wise Sticky Error */
#define IOSAV_BYTE_SERROR_C_ch(ch)	Cx(0x436c, ch) /** Byte-Wise Sticky Error Clear */

#define PM_TRML_M_CONFIG_ch(ch)		Cx(0x4380, ch) /* Thermal mode configuration */
#define PM_CMD_PWR_ch(ch)		Cx(0x4384, ch) /* Power contribution of commands */
#define PM_BW_LIMIT_CONFIG_ch(ch)	Cx(0x4388, ch) /* Bandwidth throttling on overtemp */
#define SC_WDBWM_ch(ch)			Cx(0x438c, ch) /* Watermarks and starvation counter */

/* MC Channel Broadcast registers */
#define TC_DBP			0x4c00 /* Timings: BIN */
#define TC_RAP			0x4c04 /* Timings: Regular access */
#define TC_RWP			0x4c08 /* Timings: Read / Write */
#define TC_OTHP			0x4c0c /* Timings: Other parameters */

/** WARNING: Only applies to Ivy Bridge! */
#define TC_DTP			0x4c14 /** Timings: Debug parameters */

#define SCHED_SECOND_CBIT	0x4c1c /* More chicken bits */
#define SCHED_CBIT		0x4c20 /* Chicken bits in scheduler */
#define SC_ROUNDT_LAT		0x4c24 /* Round-trip latency per rank */
#define SC_IO_LATENCY		0x4c28 /* IO Latency Configuration */
#define SCRAMBLING_SEED_1	0x4c34 /* Scrambling seed 1 */
#define SCRAMBLING_SEED_2_LO	0x4c38 /* Scrambling seed 2 low */
#define SCRAMBLING_SEED_2_HI	0x4c3c /* Scrambling seed 2 high */

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

/** Opportunistic reads configuration during write-major-mode (WMM) */
#define WMM_READ_CONFIG		0x4cd4 /** WARNING: Only exists on IVB! */

#define IOSAV_By_BW_SERROR_C(y)	Ly(0x4d40, y) /* IOSAV Bytelane Bit-wise error */

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

/**
 * Auxiliary register in mcmnts synthesis FUB (Functional Unit Block). Additionally, this
 * register is also used to enable IOSAV_n_SP_CMD_ADDR optimization on Ivy Bridge.
 */
#define MCMNTS_SPARE		0x4ea8 /** WARNING: Reserved, use only on IVB! */

#define IOSAV_ERROR		0x4eac /* Data vector count of the first error */
#define IOSAV_DC_MASK		0x4eb0 /* IOSAV data check masking */

#define IOSAV_By_ERROR_COUNT(y)	Ly(0x4f40, y) /* Per-byte 16-bit error counter */
#define IOSAV_G_ERROR_COUNT	0x4f64 /* Global 16-bit error counter */

/** WARNING: Only applies to Ivy Bridge! */
#define IOSAV_BYTE_SERROR	0x4f68 /** Byte-Wise Sticky Error */
#define IOSAV_BYTE_SERROR_C	0x4f6c /** Byte-Wise Sticky Error Clear */

#define PM_TRML_M_CONFIG	0x4f80 /* Thermal mode configuration */
#define PM_CMD_PWR		0x4f84 /* Power contribution of commands */
#define PM_BW_LIMIT_CONFIG	0x4f88 /* Bandwidth throttling on overtemperature */
#define SC_WDBWM		0x4f8c /* Watermarks and starvation counter config */

/* No, there's no need to get mad about the Memory Address Decoder */
#define MAD_CHNL		0x5000		/* Address Decoder Channel Configuration */
#define MAD_DIMM(ch)		Ly(0x5004, ch)	/* Channel characteristics */
#define MAD_DIMM_CH0		MAD_DIMM(0)	/* Channel 0 is at 0x5004 */
#define MAD_DIMM_CH1		MAD_DIMM(1)	/* Channel 1 is at 0x5008 */
#define MAD_DIMM_CH2		MAD_DIMM(2)	/* Channel 2 is at 0x500c (unused on SNB) */

#define MAD_ZR			0x5014	/* Address Decode Zones */
#define MCDECS_SPARE		0x5018 /* Spare register in mcdecs synthesis FUB */
#define MCDECS_CBIT		0x501c /* Chicken bits in mcdecs synthesis FUB */

#define CHANNEL_HASH		0x5024 /** WARNING: Only exists on IVB! */

#define MC_INIT_STATE_G		0x5030 /* High-level behavior in IOSAV mode */
#define MRC_REVISION		0x5034 /* MRC Revision */
#define PM_DLL_CONFIG		0x5064 /* Memory Controller I/O DLL config */
#define RCOMP_TIMER		0x5084 /* RCOMP evaluation timer register */

#define MC_LOCK			0x50fc /* Memory Controlller Lock register */

#define GFXVTBAR		0x5400 /* Base address for IGD */
#define VTVC0BAR		0x5410 /* Base address for PEG, USB, SATA, etc. */

/* On Ivy Bridge, this is used to enable Power Aware Interrupt Routing */
#define INTRDIRCTL		0x5418 /* Interrupt Redirection Control */

/* PAVP message register. Bit 0 locks PAVP settings, and bits [31..20] are an offset. */
#define PAVP_MSG		0x5500

#define MEM_TRML_ESTIMATION_CONFIG	0x5880
#define MEM_TRML_THRESHOLDS_CONFIG	0x5888
#define MEM_TRML_INTERRUPT		0x58a8

/* Some power MSRs are also represented in MCHBAR */
#define MCH_PKG_POWER_LIMIT_LO	0x59a0 /* Turbo Power Limit 1 parameters */
#define MCH_PKG_POWER_LIMIT_HI	0x59a4 /* Turbo Power Limit 2 parameters */

#define SSKPD			0x5d10 /* 64-bit scratchpad register */
#define SSKPD_HI		0x5d14
#define BIOS_RESET_CPL		0x5da8 /* 8-bit */

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

/* Finalize registers. The names come from Haswell, as the finalize sequence is the same. */
#define HDAUDRID		0x6008
#define UMAGFXCTL		0x6020
#define VDMBDFBARKVM		0x6030
#define VDMBDFBARPAVP		0x6034
#define VTDTRKLCK		0x63fc
#define REQLIM			0x6800
#define DMIVCLIM		0x7000
#define PEGCTL			0x7010 /* Bit 0 is PCIPWRGAT (clock gate all PEG controllers) */
#define CRDTCTL3		0x740c /* Minimum completion credits for PCIe/DMI */
#define CRDTCTL4		0x7410 /* Read Return Tracker credits */
#define CRDTLCK			0x77fc

#endif /* __SANDYBRIDGE_MCHBAR_REGS_H__ */
