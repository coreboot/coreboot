/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_PMIC_WRAP_COMMON_H
#define SOC_MEDIATEK_PMIC_WRAP_COMMON_H

#include <console/console.h>

#define PWRAPTAG                "[PWRAP] "
#define pwrap_err(fmt, arg ...) printk(BIOS_ERR, PWRAPTAG "ERROR,line=%d" fmt, \
				       __LINE__, ## arg)

/* external API */
s32 pwrap_wacs2(u32 write, u16 adr, u16 wdata, u16 *rdata, u32 init_check);
s32 pwrap_init(void);
static inline s32 pwrap_read(u16 addr, u16 *rdata)
{
	return pwrap_wacs2(0, addr, 0, rdata, 1);
}

static inline s32 pwrap_write(u16 addr, u16 wdata)
{
	return pwrap_wacs2(1, addr, wdata, 0, 1);
}

static inline u16 pwrap_read_field(u16 reg, u16 mask, u16 shift)
{
	u16 rdata;

	pwrap_read(reg, &rdata);
	rdata &= (mask << shift);
	rdata = (rdata >> shift);

	return rdata;
}

static inline void pwrap_write_field(u16 reg, u16 val, u16 mask, u16 shift)
{
	u16 old, new;

	pwrap_read(reg, &old);
	new = old & ~(mask << shift);
	new |= (val << shift);
	pwrap_write(reg, new);
}

/* internal API */
s32 pwrap_reset_spislv(void);

static inline s32 pwrap_read_nochk(u16 addr, u16 *rdata)
{
	return pwrap_wacs2(0, addr, 0, rdata, 0);
}

static inline s32 pwrap_write_nochk(u16 addr, u16 wdata)
{
	return pwrap_wacs2(1, addr, wdata, 0, 0);
}

/* dewrapper default value */
enum {
	DEFAULT_VALUE_READ_TEST  = 0x5aa5,
	WRITE_TEST_VALUE         = 0xa55a
};

/* timeout setting */
enum {
	TIMEOUT_READ_US        = 255,
	TIMEOUT_WAIT_IDLE_US   = 255
};

/* manual command */
enum {
	OP_WR    = 0x1,
	OP_CSH   = 0x0,
	OP_CSL   = 0x1,
	OP_OUTS  = 0x8,
};

enum {
	RDATA_WACS_RDATA_SHIFT = 0,
	RDATA_WACS_FSM_SHIFT   = 16,
	RDATA_WACS_REQ_SHIFT   = 19,
	RDATA_SYNC_IDLE_SHIFT,
	RDATA_INIT_DONE_V1_SHIFT,
	RDATA_SYS_IDLE_V1_SHIFT,
};

enum {
	RDATA_INIT_DONE_V2_SHIFT  = 22, /* 8186 */
	RDATA_SYS_IDLE_V2_SHIFT   = 23, /* 8186 */
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

typedef u32 (*loop_condition_fp)(u32);

static inline u32 wait_for_fsm_vldclr(u32 x)
{
	return ((x >> RDATA_WACS_FSM_SHIFT) & RDATA_WACS_FSM_MASK) !=
		WACS_FSM_WFVLDCLR;
}

static inline u32 wait_for_sync(u32 x)
{
	return ((x >> RDATA_SYNC_IDLE_SHIFT) & RDATA_SYNC_IDLE_MASK) !=
		WACS_SYNC_IDLE;
}

static inline u32 wait_for_idle_and_sync(u32 x)
{
	return ((((x >> RDATA_WACS_FSM_SHIFT) & RDATA_WACS_FSM_MASK) !=
		WACS_FSM_IDLE) || (((x >> RDATA_SYNC_IDLE_SHIFT) &
		RDATA_SYNC_IDLE_MASK) != WACS_SYNC_IDLE));
}

static inline u32 wait_for_cipher_ready(u32 x)
{
	return x != 3;
}

u32 wait_for_state_idle(u32 timeout_us, void *wacs_register,
			void *wacs_vldclr_register, u32 *read_reg);

u32 wait_for_state_ready(loop_condition_fp fp, u32 timeout_us,
			 void *wacs_register, u32 *read_reg);

#endif /* SOC_MEDIATEK_PMIC_WRAP_COMMON_H */
