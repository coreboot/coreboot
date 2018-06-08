/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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

#ifndef SOC_MEDIATEK_MT8173_PMIC_WRAP_H
#define SOC_MEDIATEK_MT8173_PMIC_WRAP_H

#include <soc/addressmap.h>
#include <types.h>

/* external API */
s32 pwrap_read(u16 adr, u16 *rdata);
s32 pwrap_write(u16 adr, u16 wdata);
s32 pwrap_wacs2(u32 write, u16 adr, u16 wdata, u16 *rdata, u32 init_check);
s32 pwrap_init(void);

static struct mt8173_pwrap_regs *const mt8173_pwrap = (void *)PMIC_WRAP_BASE;

enum {
	WACS2 = 1 << 4
};

enum {
	DEW_BASE = 0xBC00
};

/* PMIC registers */
enum {
	PMIC_BASE = 0x0000,
	PMIC_WRP_CKPDN       = PMIC_BASE + 0x011A,          // 0x0056
	PMIC_WRP_RST_CON     = PMIC_BASE + 0x0120,          // 0x005C
	PMIC_TOP_CKCON2      = PMIC_BASE + 0x012A,
	PMIC_TOP_CKCON3      = PMIC_BASE + 0x01D4
};

/* timeout setting */
enum {
	TIMEOUT_READ_US        = 255,
	TIMEOUT_WAIT_IDLE_US   = 255
};

/* PMIC_WRAP registers */
struct mt8173_pwrap_regs {
	u32 mux_sel;
	u32 wrap_en;
	u32 dio_en;
	u32 sidly;
	u32 rddmy;
	u32 si_ck_con;
	u32 cshext_write;
	u32 cshext_read;
	u32 cslext_start;
	u32 cslext_end;
	u32 staupd_prd;
	u32 staupd_grpen;
	u32 reserved[4];
	u32 staupd_man_trig;
	u32 staupd_sta;
	u32 wrap_sta;
	u32 harb_init;
	u32 harb_hprio;
	u32 hiprio_arb_en;
	u32 harb_sta0;
	u32 harb_sta1;
	u32 man_en;
	u32 man_cmd;
	u32 man_rdata;
	u32 man_vldclr;
	u32 wacs0_en;
	u32 init_done0;
	u32 wacs0_cmd;
	u32 wacs0_rdata;
	u32 wacs0_vldclr;
	u32 wacs1_en;
	u32 init_done1;
	u32 wacs1_cmd;
	u32 wacs1_rdata;
	u32 wacs1_vldclr;
	u32 wacs2_en;
	u32 init_done2;
	u32 wacs2_cmd;
	u32 wacs2_rdata;
	u32 wacs2_vldclr;
	u32 int_en;
	u32 int_flg_raw;
	u32 int_flg;
	u32 int_clr;
	u32 sig_adr;
	u32 sig_mode;
	u32 sig_value;
	u32 sig_errval;
	u32 crc_en;
	u32 timer_en;
	u32 timer_sta;
	u32 wdt_unit;
	u32 wdt_src_en;
	u32 wdt_flg;
	u32 debug_int_sel;
	u32 dvfs_adr0;
	u32 dvfs_wdata0;
	u32 dvfs_adr1;
	u32 dvfs_wdata1;
	u32 dvfs_adr2;
	u32 dvfs_wdata2;
	u32 dvfs_adr3;
	u32 dvfs_wdata3;
	u32 dvfs_adr4;
	u32 dvfs_wdata4;
	u32 dvfs_adr5;
	u32 dvfs_wdata5;
	u32 dvfs_adr6;
	u32 dvfs_wdata6;
	u32 dvfs_adr7;
	u32 dvfs_wdata7;
	u32 spminf_sta;
	u32 cipher_key_sel;
	u32 cipher_iv_sel;
	u32 cipher_en;
	u32 cipher_rdy;
	u32 cipher_mode;
	u32 cipher_swrst;
	u32 dcm_en;
	u32 dcm_dbc_prd;
};

check_member(mt8173_pwrap_regs, dcm_dbc_prd, 0x148);

enum {
	RDATA_WACS_RDATA_SHIFT = 0,
	RDATA_WACS_FSM_SHIFT   = 16,
	RDATA_WACS_REQ_SHIFT   = 19,
	RDATA_SYNC_IDLE_SHIFT,
	RDATA_INIT_DONE_SHIFT,
	RDATA_SYS_IDLE_SHIFT,
};

enum {
	RDATA_WACS_RDATA_MASK = 0xffff,
	RDATA_WACS_FSM_MASK   = 0x7,
	RDATA_WACS_REQ_MASK   = 0x1,
	RDATA_SYNC_IDLE_MASK  = 0x1,
	RDATA_INIT_DONE_MASK  = 0x1,
	RDATA_SYS_IDLE_MASK   = 0x1,
};

/* WACS_FSM */
enum {
	WACS_FSM_IDLE     = 0x00,
	WACS_FSM_REQ      = 0x02,
	WACS_FSM_WFDLE    = 0x04, /* wait for dle, wait for read data done */
	WACS_FSM_WFVLDCLR = 0x06, /* finish read data, wait for valid flag
				   * clearing */
	WACS_INIT_DONE    = 0x01,
	WACS_SYNC_IDLE    = 0x01,
	WACS_SYNC_BUSY    = 0x00
};

/* dewrapper regsister */
enum {
	DEW_EVENT_OUT_EN   = DEW_BASE + 0x0,
	DEW_DIO_EN         = DEW_BASE + 0x2,
	DEW_EVENT_SRC_EN   = DEW_BASE + 0x4,
	DEW_EVENT_SRC      = DEW_BASE + 0x6,
	DEW_EVENT_FLAG     = DEW_BASE + 0x8,
	DEW_READ_TEST      = DEW_BASE + 0xA,
	DEW_WRITE_TEST     = DEW_BASE + 0xC,
	DEW_CRC_EN         = DEW_BASE + 0xE,
	DEW_CRC_VAL        = DEW_BASE + 0x10,
	DEW_MON_GRP_SEL    = DEW_BASE + 0x12,
	DEW_MON_FLAG_SEL   = DEW_BASE + 0x14,
	DEW_EVENT_TEST     = DEW_BASE + 0x16,
	DEW_CIPHER_KEY_SEL = DEW_BASE + 0x18,
	DEW_CIPHER_IV_SEL  = DEW_BASE + 0x1A,
	DEW_CIPHER_LOAD    = DEW_BASE + 0x1C,
	DEW_CIPHER_START   = DEW_BASE + 0x1E,
	DEW_CIPHER_RDY     = DEW_BASE + 0x20,
	DEW_CIPHER_MODE    = DEW_BASE + 0x22,
	DEW_CIPHER_SWRST   = DEW_BASE + 0x24,
	DEW_CIPHER_IV0     = DEW_BASE + 0x26,
	DEW_CIPHER_IV1     = DEW_BASE + 0x28,
	DEW_CIPHER_IV2     = DEW_BASE + 0x2A,
	DEW_CIPHER_IV3     = DEW_BASE + 0x2C,
	DEW_CIPHER_IV4     = DEW_BASE + 0x2E,
	DEW_CIPHER_IV5     = DEW_BASE + 0x30
};

/* dewrapper defaule value */
enum {
	DEFAULT_VALUE_READ_TEST  = 0x5aa5,
	WRITE_TEST_VALUE         = 0xa55a
};

enum pmic_regck {
	REG_CLOCK_18MHZ,
	REG_CLOCK_26MHZ,
	REG_CLOCK_SAFE_MODE
};

/* manual commnd */
enum {
	OP_WR    = 0x1,
	OP_CSH   = 0x0,
	OP_CSL   = 0x1,
	OP_OUTS  = 0x8,
	OP_OUTD  = 0x9,
	OP_INS   = 0xC,
	OP_IND   = 0xD
};

/* error information flag */
enum {
	E_PWR_INVALID_ARG             = 1,
	E_PWR_INVALID_RW              = 2,
	E_PWR_INVALID_ADDR            = 3,
	E_PWR_INVALID_WDAT            = 4,
	E_PWR_INVALID_OP_MANUAL       = 5,
	E_PWR_NOT_IDLE_STATE          = 6,
	E_PWR_NOT_INIT_DONE           = 7,
	E_PWR_NOT_INIT_DONE_READ      = 8,
	E_PWR_WAIT_IDLE_TIMEOUT       = 9,
	E_PWR_WAIT_IDLE_TIMEOUT_READ  = 10,
	E_PWR_INIT_SIDLY_FAIL         = 11,
	E_PWR_RESET_TIMEOUT           = 12,
	E_PWR_TIMEOUT                 = 13,
	E_PWR_INIT_RESET_SPI          = 20,
	E_PWR_INIT_SIDLY              = 21,
	E_PWR_INIT_REG_CLOCK          = 22,
	E_PWR_INIT_ENABLE_PMIC        = 23,
	E_PWR_INIT_DIO                = 24,
	E_PWR_INIT_CIPHER             = 25,
	E_PWR_INIT_WRITE_TEST         = 26,
	E_PWR_INIT_ENABLE_CRC         = 27,
	E_PWR_INIT_ENABLE_DEWRAP      = 28,
	E_PWR_INIT_ENABLE_EVENT       = 29,
	E_PWR_READ_TEST_FAIL          = 30,
	E_PWR_WRITE_TEST_FAIL         = 31,
	E_PWR_SWITCH_DIO              = 32
};

#endif /* SOC_MEDIATEK_MT8173_PMIC_WRAP_H */
