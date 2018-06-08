/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2013-2015, NVIDIA CORPORATION.  All rights reserved.
 * Copyright (C) 2014 Google Inc.
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

#ifndef __SOC_NVIDIA_TEGRA210_EMC_H__
#define __SOC_NVIDIA_TEGRA210_EMC_H__

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

	EMC_NOP_CMD_SHIFT = 0,
	EMC_NOP_CMD_MASK = 1 << EMC_NOP_CMD_SHIFT,
	EMC_NOP_DEV_SELECTN_SHIFT = 30,
	EMC_NOP_DEV_SELECTN_MASK = 3 << EMC_NOP_DEV_SELECTN_SHIFT,

	EMC_TIMING_CONTROL_TIMING_UPDATE = 1,

	EMC_PIN_GPIOEN_SHIFT = 16,
	EMC_PIN_GPIO_SHIFT = 12,
	EMC_PMACRO_BRICK_CTRL_RFU1_RESET_VAL = 0x1FFF1FFF,

	AUTOCAL_MEASURE_STALL_ENABLE = 1 << 9,
	WRITE_MUX_ACTIVE = 1 << 1,
	CFG_ADR_EN_LOCKED = 1 << 1,
};

struct tegra_emc_regs {
	uint32_t intstatus;		/* 0x0 */
	uint32_t intmask;		/* 0x4 */
	uint32_t dbg;			/* 0x8 */
	uint32_t cfg;			/* 0xc */
	uint32_t adr_cfg;		/* 0x10 */
	uint32_t rsvd_0x14[3];		/* 0x14-0x1C */

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
	uint32_t tppd;			/* 0xac */
	uint32_t odt_write;		/* 0xb0 */
	uint32_t pdex2mrr;		/* 0xb4 */
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
	uint32_t rsvd_0x10c[2];		/* 0x10c-0x110 */

	uint32_t fbio_cfg6;		/* 0x114 */
	uint32_t pdex2cke;		/* 0x118 */
	uint32_t cke2pden;		/* 0x11C */
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
	uint32_t rsvd_0x160[81];	/* 0x160-0x2A0 */

	uint32_t auto_cal_config;	/* 0x2a4 */
	uint32_t auto_cal_interval;	/* 0x2a8 */
	uint32_t auto_cal_status;	/* 0x2ac */
	uint32_t req_ctrl;		/* 0x2b0 */
	uint32_t status;		/* 0x2b4 */
	uint32_t cfg_2;			/* 0x2b8 */
	uint32_t cfg_dig_dll;		/* 0x2bc */
	uint32_t cfg_dig_dll_period;	/* 0x2c0 */
	uint32_t dig_dll_status;	/* 0x2C4 */
	uint32_t cfg_dig_dll_1;		/* 0x2C8 */
	uint32_t rdv_mask;		/* 0x2cc */
	uint32_t wdv_mask;		/* 0x2d0 */
	uint32_t rdv_early_mask;	/* 0x2d4 */
	uint32_t rdv_early;		/* 0x2d8 */
	uint32_t auto_cal_config8;	/* 0x2DC */
	uint32_t zcal_interval;		/* 0x2e0 */
	uint32_t zcal_wait_cnt;		/* 0x2e4 */
	uint32_t zcal_mrw_cmd;		/* 0x2e8 */
	uint32_t zq_cal;		/* 0x2ec */
	uint32_t xm2cmdpadctrl;		/* 0x2f0 */
	uint32_t xm2comppadctrl3;	/* 0x2f4 */
	uint32_t auto_cal_vref_sel0;	/* 0x2f8 */
	uint32_t xm2dqspadctrl2;	/* 0x2fc */
	uint32_t auto_cal_vref_sel1;	/* 0x300 */
	uint32_t xm2dqpadctrl2;		/* 0x304 */
	uint32_t xm2clkpadctrl;		/* 0x308 */
	uint32_t xm2comppadctrl;	/* 0x30c */
	uint32_t fdpd_ctrl_dq;		/* 0x310 */
	uint32_t fdpd_ctrl_cmd;		/* 0x314 */
	uint32_t pmacro_cmd_brick_ctrl_fdpd;	/* 0x318 */
	uint32_t pmacro_data_brick_ctrl_fdpd;	/* 0x31c */
	uint32_t xm2dqspadctrl4;	/* 0x320 */
	uint32_t scratch0;		/* 0x324 */
	uint32_t rsvd_0x328[2];		/* 0x328-0x32C */

	uint32_t pmacro_brick_ctrl_rfu1; /* 0x330 */
	uint32_t pmacro_brick_ctrl_rfu2; /* 0x334 */
	uint32_t rsvd_0x338[18];	/* 0x338-0x37C */

	uint32_t cmd_mapping_cmd0_0;	/* 0x380 */
	uint32_t cmd_mapping_cmd0_1;	/* 0x384 */
	uint32_t cmd_mapping_cmd0_2;	/* 0x388 */
	uint32_t cmd_mapping_cmd1_0;	/* 0x38c */
	uint32_t cmd_mapping_cmd1_1;	/* 0x390 */
	uint32_t cmd_mapping_cmd1_2;	/* 0x394 */
	uint32_t cmd_mapping_cmd2_0;	/* 0x398 */
	uint32_t cmd_mapping_cmd2_1;	/* 0x39C */
	uint32_t cmd_mapping_cmd2_2;	/* 0x3A0 */
	uint32_t cmd_mapping_cmd3_0;	/* 0x3A4 */
	uint32_t cmd_mapping_cmd3_1;	/* 0x3A8 */
	uint32_t cmd_mapping_cmd3_2;	/* 0x3AC */
	uint32_t cmd_mapping_byte;	/* 0x3B0 */
	uint32_t tr_timing_0;		/* 0x3B4 */
	uint32_t tr_ctrl_0;		/* 0x3B8 */
	uint32_t tr_ctrl_1;		/* 0x3BC */
	uint32_t switch_back_ctrl;	/* 0x3C0 */
	uint32_t tr_rdv;		/* 0x3C4 */
	uint32_t stall_then_exe_before_clkchange;	/* 0x3c8 */
	uint32_t stall_then_exe_after_clkchange;	/* 0x3cc */
	uint32_t unstall_rw_after_clkchange;		/* 0x3d0 */
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
	uint32_t swizzle_rank0_byte_cfg; /* 0x400 */
	uint32_t swizzle_rank0_byte0;	/* 0x404 */
	uint32_t swizzle_rank0_byte1;	/* 0x408 */
	uint32_t swizzle_rank0_byte2;	/* 0x40c */
	uint32_t swizzle_rank0_byte3;	/* 0x410 */
	uint32_t swizzle_rank1_byte_cfg; /* 0x414 */
	uint32_t swizzle_rank1_byte0;	/* 0x418 */
	uint32_t swizzle_rank1_byte1;	/* 0x41c */
	uint32_t swizzle_rank1_byte2;	/* 0x420 */
	uint32_t swizzle_rank1_byte3;	/* 0x424 */
	uint32_t issue_qrst;		/* 0x428 */
	uint32_t rsvd_0x42C[5];		/* 0x42C-0x43C */
	uint32_t pmc_scratch1;		/* 0x440 */
	uint32_t pmc_scratch2;		/* 0x444 */
	uint32_t pmc_scratch3;		/* 0x448 */
	uint32_t rsvd_0x44C[3];		/* 0x44C-0x454 */
	uint32_t auto_cal_config2;	/* 0x458 */
	uint32_t auto_cal_config3;	/* 0x45c */
	uint32_t auto_cal_status2;	/* 0x460 */
	uint32_t auto_cal_channel;	/* 0x464 */
	uint32_t ibdly;			/* 0x468 */
	uint32_t obdly;			/* 0x46c */
	uint32_t rsvd_0x470[3];		/* 0x470-0x478 */

	uint32_t dsr_vttgen_drv;	/* 0x47c */
	uint32_t txdsrvttgen;		/* 0x480 */
	uint32_t xm2cmdpadctrl4;	/* 0x484 */
	uint32_t xm2cmdpadctrl5;	/* 0x488 */
	uint32_t we_duration;		/* 0x48C */
	uint32_t ws_duration;		/* 0x490 */
	uint32_t wev;			/* 0x494 */
	uint32_t wsv;			/* 0x498 */
	uint32_t cfg_3;			/* 0x49C */
	uint32_t mrw5;			/* 0x4A0 */
	uint32_t mrw6;			/* 0x4A4 */
	uint32_t mrw7;			/* 0x4A8 */
	uint32_t mrw8;			/* 0x4AC */
	uint32_t mrw9;			/* 0x4B0 */
	uint32_t mrw10;			/* 0x4B4 */
	uint32_t mrw11;			/* 0x4B8 */
	uint32_t mrw12;			/* 0x4BC */
	uint32_t mrw13;			/* 0x4C0 */
	uint32_t mrw14;			/* 0x4C4 */
	uint32_t rsvd_0x4c8[2];		/* 0x4C8-0x4CC */

	uint32_t mrw15;			/* 0x4D0 */
	uint32_t cfg_sync;		/* 0x4D4 */
	uint32_t fdpd_ctrl_cmd_no_ramp;	/* 0x4D8 */
	uint32_t rsvd_0x4dc[1];		/* 0x4DC */
	uint32_t wdv_chk;		/* 0x4E0 */
	uint32_t rsvd_0x4e4[28];	/* 0x4E4-0x550 */

	uint32_t cfg_pipe2;		/* 0x554 */
	uint32_t cfg_pipe_clk;		/* 0x558 */
	uint32_t cfg_pipe1;		/* 0x55C */
	uint32_t cfg_pipe;		/* 0x560 */
	uint32_t qpop;			/* 0x564 */
	uint32_t quse_width;		/* 0x568 */
	uint32_t puterm_width;		/* 0x56c */
	uint32_t bgbias_ctl0;		/* 0x570 */
	uint32_t auto_cal_config7;	/* 0x574 */
	uint32_t xm2comppadctrl2;	/* 0x578 */
	uint32_t comppadswctrl;		/* 0x57C */
	uint32_t refctrl2;		/* 0x580 */
	uint32_t fbio_cfg7;		/* 0x584 */
	uint32_t data_brlshft_0;	/* 0x588 */
	uint32_t data_brlshft_1;	/* 0x58C */
	uint32_t rfcpb;			/* 0x590 */
	uint32_t dqs_brlshft_0;		/* 0x594 */
	uint32_t dqs_brlshft_1;		/* 0x598 */
	uint32_t cmd_brlshft_0;		/* 0x59C */
	uint32_t cmd_brlshft_1;		/* 0x5A0 */
	uint32_t cmd_brlshft_2;		/* 0x5A4 */
	uint32_t cmd_brlshft_3;		/* 0x5A8 */
	uint32_t quse_brlshft_0;	/* 0x5AC */
	uint32_t auto_cal_config4;	/* 0x5B0 */
	uint32_t auto_cal_config5;	/* 0x5B4 */
	uint32_t quse_brlshft_1;	/* 0x5B8 */
	uint32_t quse_brlshft_2;	/* 0x5BC */
	uint32_t ccdmw;			/* 0x5C0 */
	uint32_t quse_brlshft_3;	/* 0x5C4 */
	uint32_t fbio_cfg8;		/* 0x5C8 */
	uint32_t auto_cal_config6;	/* 0x5CC */
	uint32_t protobist_config_addr_1; /* 0x5D0 */
	uint32_t protobist_config_addr_2; /* 0x5D4 */
	uint32_t protobist_misc;	/* 0x5D8 */
	uint32_t protobist_wdata_lower;	/* 0x5DC */
	uint32_t protobist_wdata_upper;	/* 0x5E0 */
	uint32_t dll_cfg0;		/* 0x5E4 */
	uint32_t dll_cfg1;		/* 0x5E8 */
	uint32_t protobist_rdata;	/* 0x5EC */
	uint32_t config_sample_delay;	/* 0x5F0 */
	uint32_t cfg_update;		/* 0x5F4 */
	uint32_t rsvd_0x5f8[2];		/* 0x5F8-0x5FC */

	uint32_t pmacro_quse_ddll_rank0_0;	/* 0x600 */
	uint32_t pmacro_quse_ddll_rank0_1;	/* 0x604 */
	uint32_t pmacro_quse_ddll_rank0_2;	/* 0x608 */
	uint32_t pmacro_quse_ddll_rank0_3;	/* 0x60C */
	uint32_t pmacro_quse_ddll_rank0_4;	/* 0x610 */
	uint32_t pmacro_quse_ddll_rank0_5;	/* 0x614 */
	uint32_t rsvd_0x618[2];		/* 0x618-0x61C */

	uint32_t pmacro_quse_ddll_rank1_0;	/* 0x620 */
	uint32_t pmacro_quse_ddll_rank1_1;	/* 0x624 */
	uint32_t pmacro_quse_ddll_rank1_2;	/* 0x628 */
	uint32_t pmacro_quse_ddll_rank1_3;	/* 0x62C */
	uint32_t pmacro_quse_ddll_rank1_4;	/* 0x630 */
	uint32_t pmacro_quse_ddll_rank1_5;	/* 0x634 */
	uint32_t rsvd_0x638[2];		/* 0x638-0x63C */

	uint32_t pmacro_ob_ddll_long_dq_rank0_0;	/* 0x640 */
	uint32_t pmacro_ob_ddll_long_dq_rank0_1;	/* 0x644 */
	uint32_t pmacro_ob_ddll_long_dq_rank0_2;	/* 0x648 */
	uint32_t pmacro_ob_ddll_long_dq_rank0_3;	/* 0x64C */
	uint32_t pmacro_ob_ddll_long_dq_rank0_4;	/* 0x650 */
	uint32_t pmacro_ob_ddll_long_dq_rank0_5;	/* 0x654 */
	uint32_t rsvd_0x658[2];		/* 0x658-0x65C */

	uint32_t pmacro_ob_ddll_long_dq_rank1_0;	/* 0x660 */
	uint32_t pmacro_ob_ddll_long_dq_rank1_1;	/* 0x664 */
	uint32_t pmacro_ob_ddll_long_dq_rank1_2;	/* 0x668 */
	uint32_t pmacro_ob_ddll_long_dq_rank1_3;	/* 0x66C */
	uint32_t pmacro_ob_ddll_long_dq_rank1_4;	/* 0x670 */
	uint32_t pmacro_ob_ddll_long_dq_rank1_5;	/* 0x674 */
	uint32_t rsvd_0x678[2];		/* 0x678-0x67C */

	uint32_t pmacro_ob_ddll_long_dqs_rank0_0;	/* 0x680 */
	uint32_t pmacro_ob_ddll_long_dqs_rank0_1;	/* 0x684 */
	uint32_t pmacro_ob_ddll_long_dqs_rank0_2;	/* 0x688 */
	uint32_t pmacro_ob_ddll_long_dqs_rank0_3;	/* 0x68C */
	uint32_t pmacro_ob_ddll_long_dqs_rank0_4;	/* 0x690 */
	uint32_t pmacro_ob_ddll_long_dqs_rank0_5;	/* 0x694 */
	uint32_t rsvd_0x698[2];		/* 0x698-0x69C */

	uint32_t pmacro_ob_ddll_long_dqs_rank1_0;	/* 0x6A0 */
	uint32_t pmacro_ob_ddll_long_dqs_rank1_1;	/* 0x6A4 */
	uint32_t pmacro_ob_ddll_long_dqs_rank1_2;	/* 0x6A8 */
	uint32_t pmacro_ob_ddll_long_dqs_rank1_3;	/* 0x6AC */
	uint32_t pmacro_ob_ddll_long_dqs_rank1_4;	/* 0x6B0 */
	uint32_t pmacro_ob_ddll_long_dqs_rank1_5;	/* 0x6B4 */
	uint32_t rsvd_0x6B8[2];		/* 0x6B8-0x6BC */

	uint32_t pmacro_ib_ddll_long_dqs_rank0_0;	/* 0x6C0 */
	uint32_t pmacro_ib_ddll_long_dqs_rank0_1;	/* 0x6C4 */
	uint32_t pmacro_ib_ddll_long_dqs_rank0_2;	/* 0x6C8 */
	uint32_t pmacro_ib_ddll_long_dqs_rank0_3;	/* 0x6CC */
	uint32_t pmacro_ib_ddll_long_dqs_rank0_4;	/* 0x6D0 */
	uint32_t pmacro_ib_ddll_long_dqs_rank0_5;	/* 0x6D4 */
	uint32_t rsvd_0x6D8[2];		/* 0x6D8-0x6DC */

	uint32_t pmacro_ib_ddll_long_dqs_rank1_0;	/* 0x6E0 */
	uint32_t pmacro_ib_ddll_long_dqs_rank1_1;	/* 0x6E4 */
	uint32_t pmacro_ib_ddll_long_dqs_rank1_2;	/* 0x6E8 */
	uint32_t pmacro_ib_ddll_long_dqs_rank1_3;	/* 0x6EC */
	uint32_t pmacro_ib_ddll_long_dqs_rank1_4;	/* 0x6F0 */
	uint32_t pmacro_ib_ddll_long_dqs_rank1_5;	/* 0x6F4 */
	uint32_t rsvd_0x6F8[2];		/* 0x6F8-0x6FC */

	uint32_t pmacro_autocal_cfg0;	/* 0x700 */
	uint32_t pmacro_autocal_cfg1;	/* 0x704 */
	uint32_t pmacro_autocal_cfg2;	/* 0x708 */
	uint32_t rsvd_0x70C[5];		/* 0x70C-0x71C */

	uint32_t pmacro_tx_pwrd_0;	/* 0x720 */
	uint32_t pmacro_tx_pwrd_1;	/* 0x724 */
	uint32_t pmacro_tx_pwrd_2;	/* 0x728 */
	uint32_t pmacro_tx_pwrd_3;	/* 0x72C */
	uint32_t pmacro_tx_pwrd_4;	/* 0x730 */
	uint32_t pmacro_tx_pwrd_5;	/* 0x734 */
	uint32_t rsvd_0x738[2];		/* 0x738-0x73C */

	uint32_t pmacro_tx_sel_clk_src_0;	/* 0x740 */
	uint32_t pmacro_tx_sel_clk_src_1;	/* 0x744 */
	uint32_t pmacro_tx_sel_clk_src_2;	/* 0x748 */
	uint32_t pmacro_tx_sel_clk_src_3;	/* 0x74C */
	uint32_t pmacro_tx_sel_clk_src_4;	/* 0x750 */
	uint32_t pmacro_tx_sel_clk_src_5;	/* 0x754 */
	uint32_t rsvd_0x758[2];		/* 0x758-0x75C */

	uint32_t pmacro_ddll_bypass;	/* 0x760 */
	uint32_t rsvd_0x764[3];		/* 0x764-0x76C */

	uint32_t pmacro_ddll_pwrd_0;	/* 0x770 */
	uint32_t pmacro_ddll_pwrd_1;	/* 0x774 */
	uint32_t pmacro_ddll_pwrd_2;	/* 0x778 */
	uint32_t rsvd_0x77C[1];		/* 0x77C */
	uint32_t pmacro_cmd_ctrl_0;	/* 0x780 */
	uint32_t pmacro_cmd_ctrl_1;	/* 0x784 */
	uint32_t pmacro_cmd_ctrl_2;	/* 0x788 */
	uint32_t rsvd_0x78C[277];	/* 0x78C-0xBDC */

	uint32_t pmacro_ib_vref_dq_0;		/* 0xBE0 */
	uint32_t pmacro_ib_vref_dq_1;		/* 0xBE4 */
	uint32_t pmacro_ib_vref_dq_2;		/* 0xBE8 */
	uint32_t rsvd_0xBEC[1];			/* 0xBEC */
	uint32_t pmacro_ib_vref_dqs_0;		/* 0xBF0 */
	uint32_t pmacro_ib_vref_dqs_1;		/* 0xBF4 */
	uint32_t pmacro_ib_vref_dqs_2;		/* 0xBF8 */
	uint32_t rsvd_0xBFC[1];			/* 0xBFC */
	uint32_t pmacro_ddll_long_cmd_0;	/* 0xC00 */
	uint32_t pmacro_ddll_long_cmd_1;	/* 0xC04 */
	uint32_t pmacro_ddll_long_cmd_2;	/* 0xC08 */
	uint32_t pmacro_ddll_long_cmd_3;	/* 0xC0C */
	uint32_t pmacro_ddll_long_cmd_4;	/* 0xC10 */
	uint32_t pmacro_ddll_long_cmd_5;	/* 0xC14 */
	uint32_t rsvd_0xC18[2];			/* 0xC18-0xC1C */

	uint32_t pmacro_ddll_short_cmd_0;	/* 0xC20 */
	uint32_t pmacro_ddll_short_cmd_1;	/* 0xC24 */
	uint32_t pmacro_ddll_short_cmd_2;	/* 0xC28 */
	uint32_t rsvd_0xC2C[2];			/* 0xC2C-0xC30 */

	uint32_t pmacro_vttgen_ctrl0;		/* 0xC34 */
	uint32_t pmacro_vttgen_ctrl1;		/* 0xC38 */
	uint32_t pmacro_bg_bias_ctrl_0;		/* 0xC3C */
	uint32_t pmacro_pad_cfg_ctrl;		/* 0xC40 */
	uint32_t pmacro_zctrl;			/* 0xC44 */
	uint32_t pmacro_rx_term;		/* 0xC48 */
	uint32_t pmacro_cmd_tx_drv;		/* 0xC4C */
	uint32_t pmacro_cmd_pad_rx_ctrl;	/* 0xC50 */
	uint32_t pmacro_data_pad_rx_ctrl;	/* 0xC54 */
	uint32_t pmacro_cmd_rx_term_mode;	/* 0xC58 */
	uint32_t pmacro_data_rx_term_mode;	/* 0xC5C */
	uint32_t pmacro_cmd_pad_tx_ctrl;	/* 0xC60 */
	uint32_t pmacro_data_pad_tx_ctrl;	/* 0xC64 */
	uint32_t pmacro_common_pad_tx_ctrl;	/* 0xC68 */
	uint32_t rsvd_0xC6C[1];			/* 0xC6C */
	uint32_t pmacro_dq_tx_drv;		/* 0xC70 */
	uint32_t pmacro_ca_tx_drv;		/* 0xC74 */
	uint32_t pmacro_autocal_cfg_common;	/* 0xC78 */
	uint32_t rsvd_0xC7C[1];			/* 0xC7C */
	uint32_t pmacro_brick_mapping0;		/* 0xC80 */
	uint32_t pmacro_brick_mapping1;		/* 0xC84 */
	uint32_t pmacro_brick_mapping2;		/* 0xC88 */
	uint32_t rsvd_0xC8C[25];		/* 0xC8C-0xCEC */

	uint32_t pmacro_vttgen_ctrl2;		/* 0xCF0 */
	uint32_t pmacro_ib_rxrt;		/* 0xCF4 */
	uint32_t pmacro_training_ctrl0;		/* 0xCF8 */
	uint32_t pmacro_training_ctrl1;		/* 0xCFC */
} __packed;

check_member(tegra_emc_regs, pmacro_training_ctrl1, 0xCFC);

#endif /* __SOC_NVIDIA_TEGRA210_EMC_H__ */
