/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_COMMON_SRCLKEN_RC_H
#define SOC_MEDIATEK_COMMON_SRCLKEN_RC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ACK_DELAY_US			10
#define ACK_DELAY_TIMES			200
/* RC_CENTRAL_CFG1 setting */
#define RC_CENTRAL_ENABLE_VAL		1
#define RC_CENTRAL_DISABLE_VAL		0

/* register direct write */
#define IS_SPI2PMIC_SET_CLR_VAL		0
#define KEEP_RC_SPI_ACTIVE_VAL		1
#define SRCLKEN_RC_EN_SEL_VAL		0

/* RC_CENTRAL_CFG1 settle time setting */
#define VCORE_SETTLE_TIME_VAL		0x7	/* ~= 200us */
#define ULPOSC_SETTLE_TIME_VAL		0x4	/* ~= ? 150us */
#define NON_DCXO_SETTLE_TIME_VAL	0x1	/* 2^(step+5)*0x33*30.77ns~=400us */
#define DCXO_SETTLE_TIME_VAL		0x41	/* 2^(step+5)*0x87*30.77ns~= 1063us */

/* RC_CENTRAL_CFG2[8] */
#define SRCLKENAO_MODE			0
#define VREQ_MODE			1

/* RC_CENTRAL_CFG2[25] */
#define RC_32K				0
#define RC_ULPOSC1			1

/* Signal Control Mode */
#define MERGE_OR_MODE			0x0
#define BYPASS_MODE			0x1
#define MERGE_AND_MODE			BIT(1)
#define BYPASS_RC_MODE			(0x2 << 1)
#define BYPASS_OR_MODE			0x3
#define BYPASS_OTHER_MODE		(0x3 << 1)
#define ASYNC_MODE			BIT(3)
#define NO_REQ				0
#define SW_SRCLKEN_BBLPM_MSK		0x1
#define FPM_REQ				BIT(4)
#define BBLPM_REQ			BIT(5)
#define SW_SRCLKEN_FPM_MSK		0x1

/* use srlckenao to set vcore */
#define SPI_TRIG_MODE			SRCLKENAO_MODE
/* release vcore when spi request done */
#define IS_SPI_DONE_RELEASE		0
/* pmic spec under 200us */
#define SPI_CLK_SRC			RC_32K
/* RC_CENTRAL_CFG2 control mode */
/* merge with vreq */
#define VREQ_CTRL_M			BYPASS_MODE
/* merge with ulposc */
#define ULPOSC_CTRL_M_VAL		BYPASS_MODE
/* merge with pwrap_scp */
#define PWRAP_CTRL_M			MERGE_OR_MODE

/* RC_DCXO_FPM_CFG*/
#define MD0_SRCLKENO_0_MASK_B		0
#define FULL_SET_HW_MODE		0
/* RC_DCXO_FPM_CFG control mode*/
/* merge with spm */
#define DCXO_FPM_CTRL_MODE		(MERGE_OR_MODE | ASYNC_MODE)

/* RC_CENTRAL_CFG5 */
#define RC_SPMI_BYTE_LEN		0x1
#define PMIC_GROUP_ID			0xB

/* MXX_SRCLKEN_CFG settle time setting */
#define CENTROL_CNT_STEP		0x3	/* Fix in 3 */
#define DCXO_STABLE_TIME		0x70	/* ~= 700us */
#define XO_DEFAULT_STABLE_TIME		0x29	/* ~= 400us */
#define XO_MD0_STABLE_TIME		0x15	/* ~= 200us */
#define XO_MD1_STABLE_TIME		0x15	/* ~= 200us */
#define XO_MDRF_STABLE_TIME		0x3D	/* ~= 600us */

enum {
	SW_BBLPM_LOW,
	SW_BBLPM_HIGH,
};

enum {
	SW_FPM_LOW,
	SW_FPM_HIGH,
};

enum {
	DXCO_SETTLE_BLK_DIS,
	DXCO_SETTLE_BLK_EN,
};

enum {
	REQ_ACK_IMD_DIS,
	REQ_ACK_IMD_EN,
};

struct rc_config {
	bool disabled;
	bool lpm;
	bool hw_mode;
};

enum rc_ctrl_m {
	HW_MODE = 0,
	SW_MODE = 1,
	INIT_MODE = 0xff,
};

enum {
	SRLCKEN_RC_BRINGUP = 0,
	SRCLKEN_RC_DISABLE,
	SRCLKEN_RC_ENABLE,
	SRCLKEN_RC_SKIP,
};

struct subsys_rc_con {
	u32 dcxo_prd;
	u32 xo_prd;
	u32 cnt_step;
	u32 track_en;
	u32 req_ack_imd_en;
	u32 xo_soc_link_en;
	u32 sw_bblpm;
	u32 sw_fpm;
	u32 sw_rc;
	u32 bypass_cmd;
	u32 dcxo_settle_blk_en;
};

#define SUB_CTRL_CON(_id, _dcxo_prd, _xo_prd, \
		_sw_bblpm, _sw_fpm, _sw_rc, \
		_req_ack_imd_en, _bypass_cmd, \
		_dcxo_settle_blk_en) \
	[_id] = { \
		.dcxo_prd = _dcxo_prd, \
		.xo_prd = _xo_prd, \
		.cnt_step = CENTROL_CNT_STEP, \
		.track_en = 0x0, \
		.req_ack_imd_en = _req_ack_imd_en, \
		.xo_soc_link_en = 0x0, \
		.sw_bblpm = _sw_bblpm, \
		.sw_fpm = _sw_fpm, \
		.sw_rc = _sw_rc, \
		.bypass_cmd = _bypass_cmd, \
		.dcxo_settle_blk_en = _dcxo_settle_blk_en, \
	}

/* Init as SW FPM mode */
#define SUB_CTRL_CON_INIT(_id, _dcxo_prd, _xo_prd, \
			  _req_ack_imd_en, _bypass_cmd, \
			  _dcxo_settle_blk_en) \
	SUB_CTRL_CON(_id, _dcxo_prd, _xo_prd, \
		     SW_BBLPM_LOW, SW_FPM_HIGH, SW_MODE, \
		     _req_ack_imd_en, _bypass_cmd, \
		     _dcxo_settle_blk_en)

/* Init as SW LPM mode */
#define SUB_CTRL_CON_NO_INIT(_id, _dcxo_prd, _xo_prd, \
			     _req_ack_imd_en, _bypass_cmd, \
			     _dcxo_settle_blk_en) \
	SUB_CTRL_CON(_id, _dcxo_prd, _xo_prd, \
		     SW_BBLPM_LOW, SW_FPM_LOW, SW_MODE, \
		     _req_ack_imd_en, _bypass_cmd, \
		     _dcxo_settle_blk_en)

/* Normal init, SW FPM mode */
#define SUB_CTRL_CON_EN(_id, _xo_prd, _req_ack_imd_en) \
	SUB_CTRL_CON_INIT(_id, \
			  DCXO_STABLE_TIME, _xo_prd, \
			  _req_ack_imd_en, 0, DXCO_SETTLE_BLK_EN)

extern const struct rc_config rc_config[];
extern const size_t rc_config_num;

void rc_init_subsys_hw_mode(void);
void rc_init_subsys_lpm(void);

int srclken_rc_init(void);

#endif /* SOC_MEDIATEK_COMMON_SRCLKEN_RC_H */
