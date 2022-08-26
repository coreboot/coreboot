/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_I2C_RV3028C7_CHIP_H__
#define __DRIVERS_I2C_RV3028C7_CHIP_H__

#include "rv3028c7.h"

/*
 * The RTC has three different modes in how the backup voltage is used:
 * - OFF: Backup voltage not used
 * - DIRECT: Switch to backup voltage if it is higher than VDD
 * - LEVEL: Switch to backup voltage if VDD is < 2 V and VBACKUP > 2 V
 */
enum sw_mode {
	BACKUP_SW_DIRECT = 1,
	BACKUP_SW_OFF,
	BACKUP_SW_LEVEL
};

/*
 * The RTC can be used to charge a capacitor on VBACKUP.
 * There are the following modes:
 * - OFF: Do not charge the backup capacitor via VDD
 * - VIA_3K: Connect the backup capacitor via 3 k resistor with VDD
 * - VIA_5K: Connect the backup capacitor via 5 k resistor with VDD
 * - VIA_9K: Connect the backup capacitor via 9 k resistor with VDD
 * - VIA_15K: Connect the backup capacitor via 15 k resistor with VDD
 */
enum charge_mode {
	CHARGE_OFF = 0,
	CHARGE_VIA_3K,
	CHARGE_VIA_5K,
	CHARGE_VIA_9K,
	CHARGE_VIA_15K
};

struct drivers_i2c_rv3028c7_config {
	unsigned char user_weekday;	/* User day of the week to set */
	unsigned char user_day;		/* User day to set */
	unsigned char user_month;	/* User month to set */
	unsigned char user_year;	/* User year to set (2-digit) */
	unsigned char set_user_date;	/* Use user date from devicetree */
	enum sw_mode bckup_sw_mode;	/* Mode for switching between VDD and VBACKUP */
	enum charge_mode cap_charge;	/* Mode for capacitor charging */
};

#endif /* __DRIVERS_I2C_RV3028C7_CHIP_H__ */
