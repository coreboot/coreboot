/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <reg_script.h>
#include <soc/iosf.h>

#define MAKE_MASK_INCLUSIVE(msb) \
	((1ULL << (1 + (msb))) - 1)
#define MAKE_MASK(msb) \
	((1ULL << (msb)) - 1)
#define MASK_VAL(msb, lsb, val) \
	~(MAKE_MASK_INCLUSIVE(msb) & ~MAKE_MASK(lsb)), (val) << (lsb)

#define E(arg1, arg2, args) \
	REG_IOSF_RMW(IOSF_PORT_##arg1, arg2, args)

static const struct reg_script perf_power_settings[] = {
E(AUNIT,  0x18,  MASK_VAL(22,  22,  0x1)),  // ACKGATE.AMESSAGE_MSGIF
E(AUNIT,  0x18,  MASK_VAL(21,  21,  0x1)),  // ACKGATE.AREQDOWN_SCL0_ARB
E(AUNIT,  0x18,  MASK_VAL(20,  20,  0x1)),  // ACKGATE.AREQUP_MIRROR
E(AUNIT,  0x18,  MASK_VAL(19,  19,  0x1)),  // ACKGATE.AREQTAHACK
E(AUNIT,  0x18,  MASK_VAL(18,  18,  0x1)),  // ACKGATE.AREQDOWN_TAREQQ
E(AUNIT,  0x18,  MASK_VAL(17,  17,  0x1)),  // ACKGATE.AREQDOWN_CREDIT
E(AUNIT,  0x18,  MASK_VAL(16,  16,  0x1)),  // ACKGATE.ASCLUP_FAIR_ARBITER
E(AUNIT,  0x18,  MASK_VAL(15,  15,  0x1)),  // ACKGATE.AIOSFDOWN_DATA
E(AUNIT,  0x18,  MASK_VAL(14,  14,  0x1)),  // ACKGATE.ASCLUP_IOSF_ADAPTER
E(AUNIT,  0x18,  MASK_VAL(12,  12,  0x1)),  // ACKGATE.ASCLUP_CMD_QUEUE
E(AUNIT,  0x18,  MASK_VAL(11,  11,  0x1)),  // ACKGATE.ASCLUP_DATA_QUEUE
E(AUNIT,  0x18,  MASK_VAL(10,  10,  0x1)),  // ACKGATE.AREQUP_CMD_QUEUE
E(AUNIT,  0x18,  MASK_VAL(9,  9,  0x1)),  // ACKGATE.AREQUP_DATA_QUEUE
E(AUNIT,  0x18,  MASK_VAL(8,  8,  0x1)),  // ACKGATE.AREQDOWN_RSP_QUEUE
E(AUNIT,  0x18,  MASK_VAL(7,  7,  0x1)),  // ACKGATE.AREQDOWN_DATA_QUEUE
E(AUNIT,  0x18,  MASK_VAL(6,  6,  0x1)),  // ACKGATE.AIOSFDOWN_CMD_DRVR
E(AUNIT,  0x18,  MASK_VAL(5,  5,  0x1)),  // ACKGATE.AIOSFDOWN_CMD_DATA_BUFF
E(AUNIT,  0x18,  MASK_VAL(4,  4,  0x1)),  // ACKGATE.AT_REQ_QUEUE
E(AUNIT,  0x18,  MASK_VAL(3,  3,  0x1)),  // ACKGATE.AT_DATA_QUEUE
E(AUNIT,  0x18,  MASK_VAL(2,  2,  0x1)),  // ACKGATE.TA_REQ_QUEUE
E(AUNIT,  0x18,  MASK_VAL(1,  1,  0x1)),  // ACKGATE.TA_DATA_QUEUE
E(AUNIT,  0x18,  MASK_VAL(0,  0,  0x1)),  // ACKGATE.CONFIG_REGS
E(AUNIT,  0x20,  MASK_VAL(26,  24,  0x2)),  // AISOCHCTL.CHANNEL_AB_DEADLINE_EN
E(AUNIT,  0x20,  MASK_VAL(8,  0,  0x1)),  // AISOCHCTL.VC1_ISOC_CH_DEFAULT_DDLINE_DLY
E(AUNIT,  0x21,  MASK_VAL(31,  31,  0x1)),  // AVCCTL.EFFICIENT_PERF_UP_EN
E(AUNIT,  0x21,  MASK_VAL(8,  8,  0x0)),  // AVCCTL.VC_EN_PRIORITY_DNARB
E(AUNIT,  0x0C0,  MASK_VAL(11,  8,  0x4)),  // AARBCTL0.IOSF0VC2_WGT
E(AUNIT,  0x0C0,  MASK_VAL(7,  4,  0x4)),  // AARBCTL0.IOSF0VC1_WGT
E(AUNIT,  0x0C0,  MASK_VAL(3,  0,  0x4)),  // AARBCTL0.IOSF0VC0_WGT
E(BUNIT,  0x3,  MASK_VAL(29,  24,  0x4)),  // BARBCTRL0.AGENT3_WEIGHT
E(BUNIT,  0x3,  MASK_VAL(21,  16,  0x4)),  // BARBCTRL0.AGENT2_WEIGHT
E(BUNIT,  0x3,  MASK_VAL(13,  8,  0x4)),  // BARBCTRL0.AGENT1_WEIGHT
E(BUNIT,  0x3,  MASK_VAL(5,  0,  0x4)),  // BARBCTRL0.AGENT0_WEIGHT
E(BUNIT,  0x4,  MASK_VAL(29,  24,  0x4)),  // BARBCTRL1.AGENT7_WEIGHT
E(BUNIT,  0x4,  MASK_VAL(21,  16,  0x4)),  // BARBCTRL1.AGENT6_WEIGHT
E(BUNIT,  0x4,  MASK_VAL(13,  8,  0x4)),  // BARBCTRL1.AGENT5_WEIGHT
E(BUNIT,  0x4,  MASK_VAL(5,  0,  0x4)),  // BARBCTRL1.AGENT4_WEIGHT
E(BUNIT,  0x5,  MASK_VAL(21,  16,  0x4)),  // BARBCTRL2.AGENT10_WEIGHT
E(BUNIT,  0x5,  MASK_VAL(13,  8,  0x4)),  // BARBCTRL2.AGENT9_WEIGHT
E(BUNIT,  0x5,  MASK_VAL(5,  0,  0x8)),  // BARBCTRL2.AGENT8_WEIGHT
E(BUNIT,  0x7,  MASK_VAL(31,  24,  0x20)),  // BWFLUSH.FLUSH_THRSHOLD
E(BUNIT,  0x7,  MASK_VAL(15,  8,  0x0A)),  // BWFLUSH.DIRTY_LWM
E(BUNIT,  0x7,  MASK_VAL(7,  0,  0x10)),  // BWFLUSH.DIRTY_HWM
E(BUNIT,  0x8,  MASK_VAL(23,  0,  0x0)),  // BBANKMASK.BANK_MASK
E(BUNIT,  0x9,  MASK_VAL(23,  0,  0x3FFFFC)),  // BROWMASK.ROW_MASK
E(BUNIT,  0x0A,  MASK_VAL(9,  0,  0x080)),  // BRANKMASK.RANK_MASK
E(BUNIT,  0x0B,  MASK_VAL(29,  24,  0x1F)),  // BALIMIT0.AGENT3_LIMIT
E(BUNIT,  0x0B,  MASK_VAL(21,  16,  0x2F)),  // BALIMIT0.AGENT2_LIMIT
E(BUNIT,  0x0B,  MASK_VAL(13,  8,  0x2F)),  // BALIMIT0.AGENT1_LIMIT
E(BUNIT,  0x0B,  MASK_VAL(5,  0,  0x2F)),  // BALIMIT0.AGENT0_LIMIT
E(BUNIT,  0x0C,  MASK_VAL(29,  24,  0x2F)),  // BALIMIT1.AGENT7_LIMIT
E(BUNIT,  0x0C,  MASK_VAL(21,  16,  0x2F)),  // BALIMIT1.AGENT6_LIMIT
E(BUNIT,  0x0C,  MASK_VAL(13,  8,  0x2F)),  // BALIMIT1.AGENT5_LIMIT
E(BUNIT,  0x0C,  MASK_VAL(5,  0,  0x2B)),  // BALIMIT1.AGENT4_LIMIT
E(BUNIT,  0x0D,  MASK_VAL(21,  16,  0x2F)),  // BALIMIT2.AGENT10_LIMIT
E(BUNIT,  0x0D,  MASK_VAL(13,  8,  0x2F)),  // BALIMIT2.AGENT9_LIMIT
E(BUNIT,  0x0D,  MASK_VAL(5,  0,  0x2F)),  // BALIMIT2.AGENT8_LIMIT
E(BUNIT,  0x0F,  MASK_VAL(29,  28,  0x0)),  // BARES0.AGENT7_RSVD
E(BUNIT,  0x0F,  MASK_VAL(25,  24,  0x0)),  // BARES0.AGENT6_RSVD
E(BUNIT,  0x0F,  MASK_VAL(21,  20,  0x0)),  // BARES0.AGENT5_RSVD
E(BUNIT,  0x0F,  MASK_VAL(17,  16,  0x0)),  // BARES0.AGENT4_RSVD
E(BUNIT,  0x0F,  MASK_VAL(13,  12,  0x0)),  // BARES0.AGENT3_RSVD
E(BUNIT,  0x0F,  MASK_VAL(9,  8,  0x0)),  // BARES0.AGENT2_RSVD
E(BUNIT,  0x0F,  MASK_VAL(5,  4,  0x0)),  // BARES0.AGENT1_RSVD
E(BUNIT,  0x0F,  MASK_VAL(1,  0,  0x0)),  // BARES0.AGENT0_RSVD
E(BUNIT,  0x10,  MASK_VAL(9,  8,  0x0)),  // BARES1.AGENT10_RSVD
E(BUNIT,  0x10,  MASK_VAL(5,  4,  0x0)),  // BARES1.AGENT9_RSVD
E(BUNIT,  0x10,  MASK_VAL(1,  0,  0x0)),  // BARES1.AGENT8_RSVD
E(BUNIT,  0x11,  MASK_VAL(31,  22,  0x20)),  // BISOC.ENTER_SELF_REFRESH_THRSH
E(BUNIT,  0x11,  MASK_VAL(18,  18,  0x1)),  // BISOC.SR_EXIT_SYNC_EN
E(BUNIT,  0x11,  MASK_VAL(17,  12,  0x4)),  // BISOC.ENTER_SELF_REFRESH_DLY
E(BUNIT,  0x11,  MASK_VAL(11,  8,  0x8)),  // BISOC.SCHEDULER_LATENCY
E(BUNIT,  0x12,  MASK_VAL(31,  30,  0x0)),  // BCOSCAT.COS_CAT_AGENT15 and BCOSCAT.BUS_LOCK_THROTTLE_ENABLE
E(BUNIT,  0x12,  MASK_VAL(29,  28,  0x0)),  // BCOSCAT.COS_CAT_AGENT14
E(BUNIT,  0x12,  MASK_VAL(27,  26,  0x0)),  // BCOSCAT.COS_CAT_AGENT13
E(BUNIT,  0x12,  MASK_VAL(25,  24,  0x0)),  // BCOSCAT.COS_CAT_AGENT12
E(BUNIT,  0x12,  MASK_VAL(23,  22,  0x0)),  // BCOSCAT.COS_CAT_AGENT11
E(BUNIT,  0x12,  MASK_VAL(21,  20,  0x0)),  // BCOSCAT.COS_CAT_AGENT10
E(BUNIT,  0x12,  MASK_VAL(19,  18,  0x0)),  // BCOSCAT.COS_CAT_AGENT9
E(BUNIT,  0x12,  MASK_VAL(17,  16,  0x1)),  // BCOSCAT.COS_CAT_AGENT8
E(BUNIT,  0x12,  MASK_VAL(15,  14,  0x0)),  // BCOSCAT.COS_CAT_AGENT7
E(BUNIT,  0x12,  MASK_VAL(13,  12,  0x0)),  // BCOSCAT.COS_CAT_AGENT6
E(BUNIT,  0x12,  MASK_VAL(11,  10,  0x1)),  // BCOSCAT.COS_CAT_AGENT5
E(BUNIT,  0x12,  MASK_VAL(9,  8,  0x1)),  // BCOSCAT.COS_CAT_AGENT4
E(BUNIT,  0x12,  MASK_VAL(7,  6,  0x0)),  // BCOSCAT.COS_CAT_AGENT3
E(BUNIT,  0x12,  MASK_VAL(5,  4,  0x0)),  // BCOSCAT.COS_CAT_AGENT2
E(BUNIT,  0x12,  MASK_VAL(3,  2,  0x0)),  // BCOSCAT.COS_CAT_AGENT1
E(BUNIT,  0x12,  MASK_VAL(1,  0,  0x0)),  // BCOSCAT.COS_CAT_AGENT0
E(BUNIT,  0x14,  MASK_VAL(31,  31,  0x0)),  // BFLWT.DISABLE_FLUSH_WEIGHTS
E(BUNIT,  0x14,  MASK_VAL(30,  30,  0x0)),  // BFLWT.ENABLE_READ_INVALIDATE_TIMER
E(BUNIT,  0x14,  MASK_VAL(13,  8,  0x8)),  // BFLWT.WRITE_WEIGHTS
E(BUNIT,  0x14,  MASK_VAL(5,  0,  0x10)),  // BFLWT.READ_WEIGHTS
E(BUNIT,  0x16,  MASK_VAL(31,  31,  0x0)),  // BISOCWT.ENABLE_ISOC_WEIGHTS
E(BUNIT,  0x16,  MASK_VAL(13,  8,  0x3F)),  // BISOCWT.ISOC_REQUEST_WEIGHTS
E(BUNIT,  0x16,  MASK_VAL(5,  0,  0x8)),  // BISOCWT.NON_ISOC_REQUEST_WEIGHTS
E(BUNIT,  0x18,  MASK_VAL(31,  24,  0x20)),  // BSCHCTRL0.BEST_EFFORT_MAX_LATENCY
E(BUNIT,  0x18,  MASK_VAL(23,  21,  0x6)),  // BSCHCTRL0.PAGE_HIT_DELAY
E(BUNIT,  0x18,  MASK_VAL(13,  7,  0x0)),  // BSCHCTRL0.ISOC_BANK_PREFETCH
E(BUNIT,  0x18,  MASK_VAL(6,  0,  0x20)),  // BSCHCTRL0.BEST_EFFORT_BANK_PREFETCH
E(BUNIT,  0x3B,  MASK_VAL(23,  16,  0x4)),  // BDEBUG0.CASUAL_TIMER
E(BUNIT,  0x3B,  MASK_VAL(9,  9,  0x0)),  // BDEBUG0.DISABLE_BADMIT_URGENT_ISOC
E(BUNIT,  0x3B,  MASK_VAL(7,  0,  0x0A)),  // BDEBUG0.CASUAL_WATER_MARK
E(BUNIT,  0x3C,  MASK_VAL(31,  16,  0x0FFFF)),  // BDEBUG1.AGENT_WEIGHT_ENABLE
E(BUNIT,  0x3C,  MASK_VAL(2,  2,  0x0)),  // BDEBUG1.EXIT_SR_FOR_CASUAL_FLUSH
E(BUNIT,  0x3C,  MASK_VAL(1,  1,  0x0)),  // BDEBUG1.ENABLE_DRAM_SELF_RFRSH
E(BUNIT,  0x3D,  MASK_VAL(14,  14,  0x1)),  // BCTRL.BANK_STATUS_ENABLE
E(BUNIT,  0x3D,  MASK_VAL(13,  13,  0x0)),  // BCTRL.DISABLE_OWNED
E(BUNIT,  0x3D,  MASK_VAL(12,  12,  0x0)),  // BCTRL.INORDER_READ_ENABLE
E(BUNIT,  0x3D,  MASK_VAL(11,  11,  0x0)),  // BCTRL.INORDER_FLUSH_ENABLE
E(BUNIT,  0x3D,  MASK_VAL(8,  8,  0x0)),  // BCTRL.MISS_VALID_ENTRIES
E(BUNIT,  0x3D,  MASK_VAL(7,  7,  0x0)),  // BCTRL.DIRTY_STALL
E(BUNIT,  0x3D,  MASK_VAL(6,  6,  0x0)),  // BCTRL.SINGLE_TAG_ACCESS
E(BUNIT,  0x3D,  MASK_VAL(5,  5,  0x0)),  // BCTRL.SINGLE_CHUNK_ACCESS
E(BUNIT,  0x3D,  MASK_VAL(2,  2,  0x1)),  // BCTRL.BECLK_GATE_EN
E(BUNIT,  0x3D,  MASK_VAL(1,  1,  0x1)),  // BCTRL.MASTERCLK_GATE_EN
E(BUNIT,  0x3D,  MASK_VAL(0,  0,  0x1)),  // BCTRL.REQUESTCLK_GATE_EN
E(BUNIT,  0x3E,  MASK_VAL(31,  16,  0x0)),  // BTHCTRL.AGENT_THROTTLING_ENABLE
E(BUNIT,  0x3E,  MASK_VAL(7,  0,  0x0)),  // BTHCTRL.RANK_SELECTION_MASK
E(BUNIT,  0x3F,  MASK_VAL(31,  24,  0x0FF)),  // BTHMASK.ORWRITE_MASK
E(BUNIT,  0x3F,  MASK_VAL(23,  16,  0x0FF)),  // BTHMASK.ORREAD_MASK
E(BUNIT,  0x3F,  MASK_VAL(15,  8,  0x0FF)),  // BTHMASK.ERWRITE_MASK
E(BUNIT,  0x3F,  MASK_VAL(7,  0,  0x0FF)),  // BTHMASK.ERREAD_MASK
  //0x02,  0x0,   2,   0,  0x1,  //T_INTR_REDIR_CTL.REDIR_MODE_SEL
E(CPU_BUS,  0x3,  MASK_VAL(20,  20,  0x1)),  // T_CTL.SPLIT_GOIWP_MODE
E(CPU_BUS,  0x3,  MASK_VAL(19,  19,  0x0)),  // T_CTL.DISABLE_TRDY_RDGO
E(CPU_BUS,  0x3,  MASK_VAL(18,  18,  0x0)),  // T_CTL.DISABLE_ISOC_HIGHPRI_RDDATA_RETURN
E(CPU_BUS,  0x3,  MASK_VAL(17,  17,  0x0)),  // T_CTL.ENABLE_NPC_COLLECTOR
E(CPU_BUS,  0x3,  MASK_VAL(16,  16,  0x1)),  // T_CTL.ENABLE_IN_ORDER_APIC
E(CPU_BUS,  0x3,  MASK_VAL(15,  15,  0x0)),  // T_CTL.TG_HIGHPRI_WRITE_PULLS
  //0x02,  0x3,  12,  12,  0x1,  // T_CTL.TG_NDRAMSNP
E(CPU_BUS,  0x3,  MASK_VAL(10,  10,  0x1)),  // T_CTL.TG_DW_POST_PUSH_LOG
E(CPU_BUS,  0x3,  MASK_VAL(3,  3,  0x0)),  // T_CTL.ALWAYS_SNP_IDI
E(CPU_BUS,  0x3,  MASK_VAL(2,  2,  0x0)),  // T_CTL.DIS_LIVE_BRAM_BYP_IDI
E(CPU_BUS,  0x4,  MASK_VAL(18,  18,  0x1)),  // T_MISC_CTL.DISABLE_IOSF_OUTBOUND_THROTTLE
E(CPU_BUS,  0x4,  MASK_VAL(4,  1,  0x8)),  // T_MISC_CTL.DPTE_CNT
E(CPU_BUS,  0x4,  MASK_VAL(0,  0,  0x0)),  // T_MISC_CTL.DPTE_EN
E(CPU_BUS,  0x5,  MASK_VAL(27,  27,  0x1)),  // T_CLKGATE_CTL.XUNIT_4_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(26,  26,  0x1)),  // T_CLKGATE_CTL.XUNIT_3_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(25,  25,  0x1)),  // T_CLKGATE_CTL.XUNIT_2_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(24,  24,  0x1)),  // T_CLKGATE_CTL.XUNIT_1_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(23,  23,  0x1)),  // T_CLKGATE_CTL.MON_LOG_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(22,  22,  0x1)),  // T_CLKGATE_CTL.A2T_Q_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(21,  21,  0x1)),  // T_CLKGATE_CTL.T2A_Q_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(20,  20,  0x1)),  // T_CLKGATE_CTL.A2TAPIC_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(19,  19,  0x1)),  // T_CLKGATE_CTL.B2X_DATSEL_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(18,  18,  0x1)),  // T_CLKGATE_CTL.X2B_DATSEL_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(17,  17,  0x1)),  // T_CLKGATE_CTL.S2C_RESP_SEL_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(16,  16,  0x1)),  // T_CLKGATE_CTL.T2A_REQ_SEL_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(15,  15,  0x1)),  // T_CLKGATE_CTL.C2APIC_FIFO_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(14,  14,  0x1)),  // T_CLKGATE_CTL.S2C_REQ_FIFO_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(13,  13,  0x1)),  // T_CLKGATE_CTL.S2C_REQ_SEL_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(12,  12,  0x1)),  // T_CLKGATE_CTL.TRKR_SB_LLST_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(11,  11,  0x1)),  // T_CLKGATE_CTL.TRKR_SB_OLDST_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(10,  10,  0x1)),  // T_CLKGATE_CTL.TRKR_SB_S2C_RESP_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(9,  9,  0x1)),  // T_CLKGATE_CTL.TRKR_SB_T2A_REQSTAT_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(8,  8,  0x1)),  // T_CLKGATE_CTL.TRKR_SB_B2X_DATSTAT_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(7,  7,  0x1)),  // T_CLKGATE_CTL.TRKR_SB_WRSTAT_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(6,  6,  0x1)),  // T_CLKGATE_CTL.TRKR_SB_SNP_STAT_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(5,  5,  0x1)),  // T_CLKGATE_CTL.TRKR_SB_REQ_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(4,  4,  0x1)),  // T_CLKGATE_CTL.TRKR_SB_VIOL_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(3,  3,  0x1)),  // T_CLKGATE_CTL.TRKR_SB_VALID_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(2,  2,  0x1)),  // T_CLKGATE_CTL.TRKR_SB_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(1,  1,  0x1)),  // T_CLKGATE_CTL.IOSF_SB_CFG_REG_CLK_GATE_EN
E(CPU_BUS,  0x5,  MASK_VAL(0,  0,  0x1)),  // T_CLKGATE_CTL.IOSF_SB_MSG_CLK_GATE_EN
E(0x58,  0x40,  MASK_VAL(4,  4,  0x0)),  // SSCR2.ACG_EN
E(0x58,  0x40,  MASK_VAL(4,  4,  0x0)),  // SSCR2.ACG_EN
E(0x58,  0x40,  MASK_VAL(4,  4,  0x0)),  // SSCR2.ACG_EN
E(0x55,  0x54,  MASK_VAL(1,  0,  0x0)),  // SMB_Config_PMCSR.PS
E(0x55,  0x0FC,  MASK_VAL(17,  17,  0x0)),  // SMB_Config_CGC.FUNC_CLK_CGD
E(0x55,  0x0FC,  MASK_VAL(9,  9,  0x0)),  // SMB_Config_CGC.SB_LOCAL_CGD
E(0xa2,  0x0C000,  MASK_VAL(0,  0,  0x0)),  // power_options.clkgate_disable
E(0x47,  0x0C000,  MASK_VAL(0,  0,  0x0)),  // power_options.clkgate_disable
E(0x45,  0x0C000,  MASK_VAL(0,  0,  0x0)),  // power_options.clkgate_disable
E(0x46,  0x0C000,  MASK_VAL(0,  0,  0x0)),  // power_options.clkgate_disable
E(PMC,  0x0,  MASK_VAL(11,  11,  0x1)),  // PUNIT_CONTROL.MODE_DEMOTE_EN
E(PMC,  0x0,  MASK_VAL(10,  10,  0x1)),  // PUNIT_CONTROL.CORE_DEMOTE_EN

  //
  //s0ix_PnP_Settings
  //
E(0x58,  0x1e0,    MASK_VAL(4,    4,    0x1)),    //vlv.audio.lpe.bridge.pmctl.iosfprim_trunk_gate_en
E(0x58,  0x1e0,    MASK_VAL(0,    0,    0x0)),    //vlv.audio.lpe.bridge.pmctl.iosfprimclk_gate_en
E(0x58,  0x1e0,    MASK_VAL(5,    5,    0x1)),    //vlv.audio.lpe.bridge.pmctl.iosfsb_trunk_gate_en
E(0x58,  0x1e0,    MASK_VAL(3,    3,    0x1)),    //vlv.audio.lpe.bridge.pmctl.pmctl.iosfsbclk_gate_en
E(0x58,  0x1e0,    MASK_VAL(1,    1,    0x1)),    //vlv.audio.lpe.bridge.pmctl.ocpclk_gate_en
E(0x58,  0x1e0,    MASK_VAL(2,    2,    0x1)),    //vlv.audio.lpe.bridge.pmctl.ocpclk_trunk_gate_en
E(CCU,  0x28,    MASK_VAL(31,    0,    0x0)),    //vlv.ccu.ccu_trunk_clkgate
E(CCU,  0x38,    MASK_VAL(31,    0,    0x0)),    //vlv.ccu.ccu_trunk_clkgate_2
E(CCU,  0x1c,    MASK_VAL(29,   28,    0x0)),    //vlv.ccu.clkgate_en_1.cr_lpe_pri_clkgate_en
E(CCU,  0x1c,    MASK_VAL(25,   24,    0x0)),    //vlv.ccu.clkgate_en_1.cr_lpe_sb_clkgate_en
E(CCU,  0x1c,    MASK_VAL(1,    0,    0x0)),    //vlv.ccu.clkgate_en_1.lps_free_clkgate_en
E(CCU,  0x54,    MASK_VAL(17,   16,    0x0)),    //vlv.ccu.clkgate_en_3.cr_lpe_func_ip_clkgate_en
E(CCU,  0x54,    MASK_VAL(13,   12,    0x0)),    //vlv.ccu.clkgate_en_3.cr_lpe_osc_ip_clk_en
E(CCU,  0x54,    MASK_VAL(15,   14,    0x0)),    //vlv.ccu.clkgate_en_3.cr_lpe_xtal_ip_clkgate_en
E(CCU,  0x54,    MASK_VAL(26,   24,    0x0)),    //vlv.ccu.clkgate_en_3.psf_pri_clkgate_en
E(CCU,  0x24,    MASK_VAL(24,   20,    0x0)),    //vlv.ccu.iclk_clkgate_ctrl.iopcibuffen_force_on
E(0x59,  0x1e0,    MASK_VAL(4,    4,    0x1)),    //vlv.usb.xdci_otg.controller.pmctl.iosfprim_trunk_gate_en
E(0x59,  0x1e0,    MASK_VAL(0,    0,    0x1)),    //vlv.usb.xdci_otg.controller.pmctl.iosfprimclk_gate_en
E(0x59,  0x1e0,    MASK_VAL(5,    5,    0x1)),    //vlv.usb.xdci_otg.controller.pmctl.iosfsb_trunk_gate_en
E(0x59,  0x1e0,    MASK_VAL(3,    3,    0x1)),    //vlv.usb.xdci_otg.controller.pmctl.iosfsbclk_gate_en
E(0x59,  0x1e0,    MASK_VAL(1,    1,    0x1)),    //vlv.usb.xdci_otg.controller.pmctl.ocpclk_gate_en
E(0x59,  0x1e0,    MASK_VAL(2,    2,    0x1)),    //vlv.usb.xdci_otg.controller.pmctl.ocpclk_trunk_gate_en
E(0x5a,  0xd0,     MASK_VAL(8,    0,   0x3f)),    //vlv.usb.xhci.controller.usb2pr.usb2hcsel
E(0x5a,  0x40,    MASK_VAL(21,   19,    0x6)),    //vlv.usb.xhci.controller.xhcc1.iil1e
E(0x5a,  0x40,    MASK_VAL(10,    8,    0x1)),    //vlv.usb.xhci.controller.xhcc1.l23hrawc
E(0x5a,  0x40,    MASK_VAL(18,   18,    0x1)),    //vlv.usb.xhci.controller.xhcc1.xhcil1e
E(0x5a,  0x50,     MASK_VAL(3,    3,    0x1)),    //vlv.usb.xhci.controller.xhclkgten.hsltcge
E(0x5a,  0x50,     MASK_VAL(0,    0,    0x1)),    //vlv.usb.xhci.controller.xhclkgten.iosfblcge
E(0x5a,  0x50,     MASK_VAL(1,    1,    0x1)),    //vlv.usb.xhci.controller.xhclkgten.iosfbtcge
E(0x5a,  0x50,     MASK_VAL(2,    2,    0x1)),    //vlv.usb.xhci.controller.xhclkgten.ssltcge
E(0x5a,  0x50,     MASK_VAL(7,    5,    0x2)),    //vlv.usb.xhci.controller.xhclkgten.sspllsue
E(0x5a,  0x50,    MASK_VAL(13,   13,    0x1)),    //vlv.usb.xhci.controller.xhclkgten.xhcbbtcgipiso
E(0x5a,  0x50,     MASK_VAL(4,    4,    0x1)),    //vlv.usb.xhci.controller.xhclkgten.xhcblcge
E(0x5a,  0x50,    MASK_VAL(14,   14,    0x1)),    //vlv.usb.xhci.controller.xhclkgten.xhcftclkse
E(0x5a,  0x50,    MASK_VAL(12,   12,    0x0)),    //vlv.usb.xhci.controller.xhclkgten.xhchstcgu2nrwe
E(0x5a,  0x50,    MASK_VAL(11,   10,    0x3)),    //vlv.usb.xhci.controller.xhclkgten.xhcusb2pllsdle
E(SCORE,  0x4900,  MASK_VAL(16,   16,    0x1)),    //vlv.gpio.gpscore.cfio_regs_com_cfg_score_pb_config.sb_clkgaten
E(SSUS,  0x4900,  MASK_VAL(16,   16,    0x1)),    //vlv.gpio.gpssus.cfio_regs_com_cfg_ssus_pb_config.sb_clkgaten
E(LPSS,  0x180,    MASK_VAL(1,    1,    0x1)),    //vlv.lpss.iosf2ahb.pmctl.ahb_clk_gate_en
E(LPSS,  0x180,    MASK_VAL(4,    4,    0x1)),    //vlv.lpss.iosf2ahb.pmctl.ahb_trunk_gate_enable
E(LPSS,  0x180,    MASK_VAL(0,    0,    0x1)),    //vlv.lpss.iosf2ahb.pmctl.iosf_clk_gate_enable
E(LPSS,  0x180,    MASK_VAL(3,    3,    0x1)),    //vlv.lpss.iosf2ahb.pmctl.iosfprim_trunk_gate_enable
E(LPSS,  0x180,    MASK_VAL(5,    5,    0x1)),    //vlv.lpss.iosf2ahb.pmctl.iosfsb_trunk_gate_enable
E(LPSS,  0x180,    MASK_VAL(2,    2,    0x1)),    //vlv.lpss.iosf2ahb.pmctl.side_clk_gate_enable
  //0x54,  0xfc,    31,    0,    0x0,    //vlv.pcu.iosfahbep.clock_gating_control
  //0x55,  0xfc,     1,    1,    0x0,    //vlv.pcu.smbus.smb_config_cgc.pri_local_cgd
  //0x55,  0xfc,     0,    0,    0x0,    //vlv.pcu.smbus.smb_config_cgc.pri_trunk_cgd
  //0x55,  0xfc,     8,    8,    0x0,    //vlv.pcu.smbus.smb_config_cgc.sb_trunk_cgd
E(SCC,  0x600,   MASK_VAL(31,   15,    0x5)),    //vlv.scc.iosf2ocp.gen_regrw1.gen_reg_rw1
E(SCC,  0x1e0,    MASK_VAL(4,    4,    0x1)),    //vlv.scc.iosf2ocp.pmctl.iosfprim_trunk_gate_en
E(SCC,  0x1e0,    MASK_VAL(0,    0,    0x1)),    //vlv.scc.iosf2ocp.pmctl.iosfprimclk_gate_en
E(SCC,  0x1e0,    MASK_VAL(5,    5,    0x1)),    //vlv.scc.iosf2ocp.pmctl.iosfsb_trunk_gate_en
E(SCC,  0x1e0,    MASK_VAL(3,    3,    0x1)),    //vlv.scc.iosf2ocp.pmctl.iosfsbclk_gate_en
E(SCC,  0x1e0,    MASK_VAL(1,    1,    0x1)),    //vlv.scc.iosf2ocp.pmctl.ocpclk_gate_en
E(SCC,  0x1e0,    MASK_VAL(2,    2,    0x1)),    //vlv.scc.iosf2ocp.pmctl.ocpclk_trunk_gate_en
E(SEC,  0x88,     MASK_VAL(7,    7,    0x0)),    //vlv.sec.clk_gate_dis.nfc_cg_dis
E(SEC,  0x88,     MASK_VAL(1,    1,    0x0)),    //vlv.sec.clk_gate_dis.prim_cg_dis
E(SEC,  0x88,     MASK_VAL(2,    2,    0x0)),    //vlv.sec.clk_gate_dis.prim_clkreq_dis
E(SEC,  0x88,     MASK_VAL(3,    3,    0x0)),    //vlv.sec.clk_gate_dis.prim_xsm_clkreq_dis
E(SEC,  0x88,     MASK_VAL(4,    4,    0x0)),    //vlv.sec.clk_gate_dis.sap_cg_dis
E(SEC,  0x88,     MASK_VAL(6,    6,    0x0)),    //vlv.sec.clk_gate_dis.sap_clkidle_dis
E(SEC,  0x88,     MASK_VAL(5,    5,    0x0)),    //vlv.sec.clk_gate_dis.sap_ip_cg_dis
E(SEC,  0x88,     MASK_VAL(0,    0,    0x0)),    //vlv.sec.clk_gate_dis.sb_cg_dis
REG_SCRIPT_END,
};

static void perf_power(void *unused)
{
	printk(BIOS_DEBUG, "Applying perf/power settings.\n");
	reg_script_run(perf_power_settings);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, perf_power, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, perf_power, NULL);
