/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2013, NVIDIA CORPORATION.  All rights reserved.
 * Copyright (C) 2013 Google Inc.
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

#ifndef __SOC_NVIDIA_TEGRA124_EMC_H__
#define __SOC_NVIDIA_TEGRA124_EMC_H__

#include <stddef.h>
#include <stdint.h>
#include <compiler.h>

enum {
	EMC_PIN_RESET_MASK = 1 << 8,
	EMC_PIN_RESET_ACTIVE = 0 << 8,
	EMC_PIN_RESET_INACTIVE = 1 << 8,
	EMC_PIN_DQM_MASK = 1 << 4,
	EMC_PIN_DQM_NORMAL = 0 << 4,
	EMC_PIN_DQM_INACTIVE = 1 << 4,
	EMC_PIN_CKE_MASK = 1 << 0,
	EMC_PIN_CKE_POWERDOWN = 0 << 0,
	EMC_PIN_CKE_NORMAL = 1 << 0,

	EMC_REF_CMD_MASK = 1 << 0,
	EMC_REF_CMD_REFRESH = 1 << 0,
	EMC_REF_NORMAL_MASK = 1 << 1,
	EMC_REF_NORMAL_INIT = 0 << 1,
	EMC_REF_NORMAL_ENABLED = 1 << 1,
	EMC_REF_NUM_SHIFT = 8,
	EMC_REF_NUM_MASK = 0xFF << EMC_REF_NUM_SHIFT,
	EMC_REF_DEV_SELECTN_SHIFT = 30,
	EMC_REF_DEV_SELECTN_MASK = 3 << EMC_REF_DEV_SELECTN_SHIFT,

	EMC_REFCTRL_REF_VALID_MASK = 1 << 31,
	EMC_REFCTRL_REF_VALID_DISABLED = 0 << 31,
	EMC_REFCTRL_REF_VALID_ENABLED = 1 << 31,

	EMC_CFG_EMC2PMACRO_CFG_BYPASS_ADDRPIPE_MASK = 1 << 1,
	EMC_CFG_EMC2PMACRO_CFG_BYPASS_DATAPIPE1_MASK = 1 << 2,
	EMC_CFG_EMC2PMACRO_CFG_BYPASS_DATAPIPE2_MASK = 1 << 3,

	EMC_NOP_NOP_CMD_SHIFT = 0,
	EMC_NOP_NOP_CMD_MASK = 1 << EMC_NOP_NOP_CMD_SHIFT,
	EMC_NOP_NOP_DEV_SELECTN_SHIFT = 30,
	EMC_NOP_NOP_DEV_SELECTN_MASK = 3 << EMC_NOP_NOP_DEV_SELECTN_SHIFT,

	EMC_TIMING_CONTROL_TIMING_UPDATE = 1,
};

struct tegra_emc_regs {
	uint32_t intstatus;		/* 0x0 */
	uint32_t intmask;		/* 0x4 */
	uint32_t dbg;			/* 0x8 */
	uint32_t cfg;			/* 0xc */
	uint32_t adr_cfg;		/* 0x10 */
	uint32_t rsvd_0x14[3];		/* 0x14 */

	uint32_t refctrl;		/* 0x20 */
	uint32_t pin;			/* 0x24 */
	uint32_t timing_control;	/* 0x28 */
	uint32_t rc;			/* 0x2c */
	uint32_t rfc;			/* 0x30 */
	uint32_t ras;			/* 0x34 */
	uint32_t rp;			/* 0x38 */
	uint32_t r2w;			/* 0x3c */
	uint32_t w2r;			/* 0x40 */
	uint32_t r2p;			/* 0x44 */
	uint32_t w2p;			/* 0x48 */
	uint32_t rd_rcd;		/* 0x4c */
	uint32_t wr_rcd;		/* 0x50 */
	uint32_t rrd;			/* 0x54 */
	uint32_t rext;			/* 0x58 */
	uint32_t wdv;			/* 0x5c */
	uint32_t quse;			/* 0x60 */
	uint32_t qrst;			/* 0x64 */
	uint32_t qsafe;			/* 0x68 */
	uint32_t rdv;			/* 0x6c */
	uint32_t refresh;		/* 0x70 */
	uint32_t burst_refresh_num;	/* 0x74 */
	uint32_t pdex2wr;		/* 0x78 */
	uint32_t pdex2rd;		/* 0x7c */
	uint32_t pchg2pden;		/* 0x80 */
	uint32_t act2pden;		/* 0x84 */
	uint32_t ar2pden;		/* 0x88 */
	uint32_t rw2pden;		/* 0x8c */
	uint32_t txsr;			/* 0x90 */
	uint32_t tcke;			/* 0x94 */
	uint32_t tfaw;			/* 0x98 */
	uint32_t trpab;			/* 0x9c */
	uint32_t tclkstable;		/* 0xa0 */
	uint32_t tclkstop;		/* 0xa4 */
	uint32_t trefbw;		/* 0xa8 */
	uint32_t rsvd_0xac[1];		/* 0xac */
	uint32_t odt_write;		/* 0xb0 */
	uint32_t odt_read;		/* 0xb4 */
	uint32_t wext;			/* 0xb8 */
	uint32_t ctt;			/* 0xbc */
	uint32_t rfc_slr;		/* 0xc0 */
	uint32_t mrs_wait_cnt2;		/* 0xc4 */
	uint32_t mrs_wait_cnt;		/* 0xc8 */
	uint32_t mrs;			/* 0xcc */
	uint32_t emrs;			/* 0xd0 */
	uint32_t ref;			/* 0xd4 */
	uint32_t pre;			/* 0xd8 */
	uint32_t nop;			/* 0xdc */
	uint32_t self_ref;		/* 0xe0 */
	uint32_t dpd;			/* 0xe4 */
	uint32_t mrw;			/* 0xe8 */
	uint32_t mrr;			/* 0xec */
	uint32_t cmdq;			/* 0xf0 */
	uint32_t mc2emcq;		/* 0xf4 */
	uint32_t xm2dqspadctrl3;	/* 0xf8 */
	uint32_t rsvd_0xfc[1];		/* 0xfc */
	uint32_t fbio_spare;		/* 0x100 */
	uint32_t fbio_cfg5;		/* 0x104 */
	uint32_t fbio_wrptr_eq_2;	/* 0x108 */
	uint32_t rsvd_0x10c[2];		/* 0x10c */

	uint32_t fbio_cfg6;		/* 0x114 */
	uint32_t rsvd_0x118[2];		/* 0x118 */

	uint32_t cfg_rsv;		/* 0x120 */
	uint32_t acpd_control;		/* 0x124 */
	uint32_t rsvd_0x128[1];		/* 0x128 */
	uint32_t emrs2;			/* 0x12c */
	uint32_t emrs3;			/* 0x130 */
	uint32_t mrw2;			/* 0x134 */
	uint32_t mrw3;			/* 0x138 */
	uint32_t mrw4;			/* 0x13c */
	uint32_t clken_override;	/* 0x140 */
	uint32_t r2r;			/* 0x144 */
	uint32_t w2w;			/* 0x148 */
	uint32_t einput;		/* 0x14c */
	uint32_t einput_duration;	/* 0x150 */
	uint32_t puterm_extra;		/* 0x154 */
	uint32_t tckesr;		/* 0x158 */
	uint32_t tpd;			/* 0x15c */
	uint32_t rsvd_0x160[81];		/* 0x160 */

	uint32_t auto_cal_config;	/* 0x2a4 */
	uint32_t auto_cal_interval;	/* 0x2a8 */
	uint32_t auto_cal_status;	/* 0x2ac */
	uint32_t req_ctrl;		/* 0x2b0 */
	uint32_t status;		/* 0x2b4 */
	uint32_t cfg_2;			/* 0x2b8 */
	uint32_t cfg_dig_dll;		/* 0x2bc */
	uint32_t cfg_dig_dll_period;	/* 0x2c0 */
	uint32_t rsvd_0x2c4[1];		/* 0x2c4 */
	uint32_t dig_dll_status;	/* 0x2c8 */
	uint32_t rdv_mask;		/* 0x2cc */
	uint32_t wdv_mask;		/* 0x2d0 */
	uint32_t rsvd_0x2d4[1];		/* 0x2d4 */
	uint32_t ctt_duration;		/* 0x2d8 */
	uint32_t ctt_term_ctrl;		/* 0x2dc */
	uint32_t zcal_interval;		/* 0x2e0 */
	uint32_t zcal_wait_cnt;		/* 0x2e4 */
	uint32_t zcal_mrw_cmd;		/* 0x2e8 */
	uint32_t zq_cal;		/* 0x2ec */
	uint32_t xm2cmdpadctrl;		/* 0x2f0 */
	uint32_t xm2cmdpadctrl2;	/* 0x2f4 */
	uint32_t xm2dqspadctrl;		/* 0x2f8 */
	uint32_t xm2dqspadctrl2;	/* 0x2fc */
	uint32_t xm2dqpadctrl;		/* 0x300 */
	uint32_t xm2dqpadctrl2;		/* 0x304 */
	uint32_t xm2clkpadctrl;		/* 0x308 */
	uint32_t xm2comppadctrl;	/* 0x30c */
	uint32_t xm2vttgenpadctrl;	/* 0x310 */
	uint32_t xm2vttgenpadctrl2;	/* 0x314 */
	uint32_t xm2vttgenpadctrl3;	/* 0x318 */
	uint32_t emcpaden;		/* 0x31c */
	uint32_t xm2dqspadctrl4;	/* 0x320 */
	uint32_t scratch0;		/* 0x324 */
	uint32_t dll_xform_dqs0;	/* 0x328 */
	uint32_t dll_xform_dqs1;	/* 0x32c */
	uint32_t dll_xform_dqs2;	/* 0x330 */
	uint32_t dll_xform_dqs3;	/* 0x334 */
	uint32_t dll_xform_dqs4;	/* 0x338 */
	uint32_t dll_xform_dqs5;	/* 0x33c */
	uint32_t dll_xform_dqs6;	/* 0x340 */
	uint32_t dll_xform_dqs7;	/* 0x344 */
	uint32_t dll_xform_quse0;	/* 0x348 */
	uint32_t dll_xform_quse1;	/* 0x34c */
	uint32_t dll_xform_quse2;	/* 0x350 */
	uint32_t dll_xform_quse3;	/* 0x354 */
	uint32_t dll_xform_quse4;	/* 0x358 */
	uint32_t dll_xform_quse5;	/* 0x35c */
	uint32_t dll_xform_quse6;	/* 0x360 */
	uint32_t dll_xform_quse7;	/* 0x364 */
	uint32_t dll_xform_dq0;		/* 0x368 */
	uint32_t dll_xform_dq1;		/* 0x36c */
	uint32_t dll_xform_dq2;		/* 0x370 */
	uint32_t dll_xform_dq3;		/* 0x374 */
	uint32_t dli_rx_trim0;		/* 0x378 */
	uint32_t dli_rx_trim1;		/* 0x37c */
	uint32_t dli_rx_trim2;		/* 0x380 */
	uint32_t dli_rx_trim3;		/* 0x384 */
	uint32_t dli_rx_trim4;		/* 0x388 */
	uint32_t dli_rx_trim5;		/* 0x38c */
	uint32_t dli_rx_trim6;		/* 0x390 */
	uint32_t dli_rx_trim7;		/* 0x394 */
	uint32_t dli_tx_trim0;		/* 0x398 */
	uint32_t dli_tx_trim1;		/* 0x39c */
	uint32_t dli_tx_trim2;		/* 0x3a0 */
	uint32_t dli_tx_trim3;		/* 0x3a4 */
	uint32_t dli_trim_txdqs0;	/* 0x3a8 */
	uint32_t dli_trim_txdqs1;	/* 0x3ac */
	uint32_t dli_trim_txdqs2;	/* 0x3b0 */
	uint32_t dli_trim_txdqs3;	/* 0x3b4 */
	uint32_t dli_trim_txdqs4;	/* 0x3b8 */
	uint32_t dli_trim_txdqs5;	/* 0x3bc */
	uint32_t dli_trim_txdqs6;	/* 0x3c0 */
	uint32_t dli_trim_txdqs7;	/* 0x3c4 */
	uint32_t rsvd_0x3c8[1];		/* 0x3c8 */
	uint32_t stall_then_exe_after_clkchange;	/* 0x3cc */
	uint32_t rsvd_0x3d0[1];		/* 0x3d0 */
	uint32_t auto_cal_clk_status;	/* 0x3d4 */
	uint32_t sel_dpd_ctrl;		/* 0x3d8 */
	uint32_t pre_refresh_req_cnt;	/* 0x3dc */
	uint32_t dyn_self_ref_control;	/* 0x3e0 */
	uint32_t txsrdll;		/* 0x3e4 */
	uint32_t ccfifo_addr;		/* 0x3e8 */
	uint32_t ccfifo_data;		/* 0x3ec */
	uint32_t ccfifo_status;		/* 0x3f0 */
	uint32_t cdb_cntl_1;		/* 0x3f4 */
	uint32_t cdb_cntl_2;		/* 0x3f8 */
	uint32_t xm2clkpadctrl2;	/* 0x3fc */
	uint32_t swizzle_rank0_byte_cfg;	/* 0x400 */
	uint32_t swizzle_rank0_byte0;	/* 0x404 */
	uint32_t swizzle_rank0_byte1;	/* 0x408 */
	uint32_t swizzle_rank0_byte2;	/* 0x40c */
	uint32_t swizzle_rank0_byte3;	/* 0x410 */
	uint32_t swizzle_rank1_byte_cfg;	/* 0x414 */
	uint32_t swizzle_rank1_byte0;	/* 0x418 */
	uint32_t swizzle_rank1_byte1;	/* 0x41c */
	uint32_t swizzle_rank1_byte2;	/* 0x420 */
	uint32_t swizzle_rank1_byte3;	/* 0x424 */
	uint32_t ca_training_start;	/* 0x428 */
	uint32_t ca_training_busy;	/* 0x42c */
	uint32_t ca_training_cfg;	/* 0x430 */
	uint32_t ca_training_timing_cntl1;	/* 0x434 */
	uint32_t ca_training_timing_cntl2;	/* 0x438 */
	uint32_t ca_training_ca_lead_in;	/* 0x43c */
	uint32_t ca_training_ca;	/* 0x440 */
	uint32_t ca_training_ca_lead_out;	/* 0x444 */
	uint32_t ca_training_result1;	/* 0x448 */
	uint32_t ca_training_result2;	/* 0x44c */
	uint32_t ca_training_result3;	/* 0x450 */
	uint32_t ca_training_result4;	/* 0x454 */
	uint32_t auto_cal_config2;	/* 0x458 */
	uint32_t auto_cal_config3;	/* 0x45c */
	uint32_t auto_cal_status2;	/* 0x460 */
	uint32_t xm2cmdpadctrl3;	/* 0x464 */
	uint32_t ibdly;			/* 0x468 */
	uint32_t dll_xform_addr0;	/* 0x46c */
	uint32_t dll_xform_addr1;	/* 0x470 */
	uint32_t dll_xform_addr2;	/* 0x474 */
	uint32_t dli_addr_trim;		/* 0x478 */
	uint32_t dsr_vttgen_drv;	/* 0x47c */
	uint32_t txdsrvttgen;		/* 0x480 */
	uint32_t xm2cmdpadctrl4;	/* 0x484 */
	uint32_t xm2cmdpadctrl5;	/* 0x488 */
	uint32_t rsvd_0x48c[5];		/* 0x48c */

	uint32_t dll_xform_dqs8;	/* 0x4a0 */
	uint32_t dll_xform_dqs9;	/* 0x4a4 */
	uint32_t dll_xform_dqs10;	/* 0x4a8 */
	uint32_t dll_xform_dqs11;	/* 0x4ac */
	uint32_t dll_xform_dqs12;	/* 0x4b0 */
	uint32_t dll_xform_dqs13;	/* 0x4b4 */
	uint32_t dll_xform_dqs14;	/* 0x4b8 */
	uint32_t dll_xform_dqs15;	/* 0x4bc */
	uint32_t dll_xform_quse8;	/* 0x4c0 */
	uint32_t dll_xform_quse9;	/* 0x4c4 */
	uint32_t dll_xform_quse10;	/* 0x4c8 */
	uint32_t dll_xform_quse11;	/* 0x4cc */
	uint32_t dll_xform_quse12;	/* 0x4d0 */
	uint32_t dll_xform_quse13;	/* 0x4d4 */
	uint32_t dll_xform_quse14;	/* 0x4d8 */
	uint32_t dll_xform_quse15;	/* 0x4dc */
	uint32_t dll_xform_dq4;		/* 0x4e0 */
	uint32_t dll_xform_dq5;		/* 0x4e4 */
	uint32_t dll_xform_dq6;		/* 0x4e8 */
	uint32_t dll_xform_dq7;		/* 0x4ec */
	uint32_t rsvd_0x4f0[12];		/* 0x4f0 */

	uint32_t dli_trim_txdqs8;	/* 0x520 */
	uint32_t dli_trim_txdqs9;	/* 0x524 */
	uint32_t dli_trim_txdqs10;	/* 0x528 */
	uint32_t dli_trim_txdqs11;	/* 0x52c */
	uint32_t dli_trim_txdqs12;	/* 0x530 */
	uint32_t dli_trim_txdqs13;	/* 0x534 */
	uint32_t dli_trim_txdqs14;	/* 0x538 */
	uint32_t dli_trim_txdqs15;	/* 0x53c */
	uint32_t cdb_cntl_3;		/* 0x540 */
	uint32_t xm2dqspadctrl5;	/* 0x544 */
	uint32_t xm2dqspadctrl6;	/* 0x548 */
	uint32_t xm2dqpadctrl3;		/* 0x54c */
	uint32_t dll_xform_addr3;	/* 0x550 */
	uint32_t dll_xform_addr4;	/* 0x554 */
	uint32_t dll_xform_addr5;	/* 0x558 */
	uint32_t rsvd_0x55c[1];		/* 0x55c */
	uint32_t cfg_pipe;		/* 0x560 */
	uint32_t qpop;			/* 0x564 */
	uint32_t quse_width;		/* 0x568 */
	uint32_t puterm_width;		/* 0x56c */
	uint32_t bgbias_ctl0;		/* 0x570 */
	uint32_t puterm_adj;		/* 0x574 */
} __packed;

check_member(tegra_emc_regs, puterm_adj, 0x574);

#endif /* __SOC_NVIDIA_TEGRA124_EMC_H__ */
