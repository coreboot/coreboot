/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _I2C_PTN3460_H_
#define _I2C_PTN3460_H_

#include <types.h>

#define PTN_EDID_OFF		0x00
#define PTN_EDID_LEN		0x80
#define PTN_CONFIG_OFF		0x80
#define PTN_CONFIG_LEN		0x19
#define PTN_FLASH_CFG_OFF	0xE8
#define PTN_FLASH_CFG_LEN	0x04
#define PTN_MAX_EDID_NUM	6
#define PTN_ENABLE_EMULATION	(1 << 0)

/* Define some error codes that can be used */
#define PTN_SUCCESS		0x00000000
#define PTN_BUS_ERROR		0x10000000
#define PTN_INVALID_EDID	0x20000000
#define PTN_INVALID_EDID_BLOCK	0x30000000
#define PTN_ERROR		0x40000000

struct ptn_3460_config {
	u8	dp_interface_ctrl;	/* DisplayPort interface control */
	u8	lvds_interface_ctrl1;	/* LVDS interface control register 1 */
	u8	lvds_interface_ctrl2;	/* LVDS interface control register 2 */
	u8	lvds_interface_ctrl3;	/* LVDS interface control register 3 */
	u8	edid_rom_emulation;	/* select which EDID-block is emulated */
	u8	edid_rom_access_ctrl;	/* select which EDID block to map to 0..0x7F */
	u8	pwm_min[3];		/* smallest PWM frequency for back light */
	u8	pwm_max[3];		/* biggest PWM frequency for back light */
	u8	fast_link_ctrl;		/* Fast link training control register */
	u8	pin_cfg_ctrl1;		/* Pin configuration control register 1 */
	u8	pin_cfg_ctrl2;		/* Pin configuration control register 2 */
	u8	pwm_default;		/* Default PWM bit count in DPCD register */
	u16	pwm_value;		/* Current PWM bit count in DPCD register */
	u8	pwm_default_freq;	/* Default PWM frequency in DPCD register */
	u8	t3_timing;		/* Panel T3 timing value */
	u8	t12_timing;		/* Panel T12 timing value */
	u8	backlight_ctrl;		/* Back light control register */
	u8	t2_delay;		/* Panel T2 delay */
	u8	t4_timing;		/* Panel T4 timing value */
	u8	t5_delay;		/* Panel T5 delay */
} __packed;

struct ptn_3460_flash {
	u8	cmd;			/* Flash command (erase or erase and flash) */
	u16	magic;			/* Magic number needed by the flash algorithm */
	u8	trigger;		/* Trigger for starting flash operation */
} __packed;

/* We need functions which we can call to get mainboard specific data */
/* These functions can be implemented somewhere else but must exist. */
extern enum cb_err mb_get_edid(uint8_t edid_data[PTN_EDID_LEN]);
extern uint8_t mb_select_edid_table(void);
extern enum cb_err mb_adjust_cfg(struct ptn_3460_config *cfg_ptr);

#endif /* _I2C_PTN3460_H_ */
