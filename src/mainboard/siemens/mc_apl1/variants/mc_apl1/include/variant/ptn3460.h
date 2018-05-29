/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014-2017 Siemens AG
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

#ifndef PTN3460_H_
#define PTN3460_H_

#include <stdint.h>
#include <compiler.h>

#define PTN_SLAVE_ADR		0x20
#define PTN_I2C_CONTROLLER	0

#define PTN_EDID_OFF		0x00
#define PTN_EDID_LEN		0x80
#define PTN_CONFIG_OFF		0x80
#define PTN_FLASH_CFG_OFF	0xE8
#define PTN_FLASH_CFG_LEN	0x04
#define PTN_MAX_EDID_NUM	6

/* Define some error codes that can be used. */
#define PTN_NO_ERROR		0x00000000
#define PTN_BUS_ERROR		0x10000000
#define PTN_INVALID_EDID	0x20000000

struct ptn_3460_config {
	/* DiplayPort interface control. */
	uint8_t dp_interface_ctrl;
	/* LVDS interface control register 1. */
	uint8_t lvds_interface_ctrl1;
	/* LVDS interface control register 2. */
	uint8_t lvds_interface_ctrl2;
	/* LVDS interface control register 3. */
	uint8_t lvds_interface_ctrl3;
	/* Select which EDID-block is emulated. */
	uint8_t edid_rom_emulation;
	/* Select which EDID block to map to 0..0x7F. */
	uint8_t edid_rom_access_ctrl;
	/* Smallest PWM frequency for back light. */
	uint8_t pwm_min[3];
	/* Biggest PWM frequency for back light. */
	uint8_t pwm_max[3];
	/* Fast link training control register. */
	uint8_t fast_link_ctrl;
	/* Pin configuration control register 1. */
	uint8_t pin_cfg_ctrl1;
	/* Pin configuration control register 2. */
	uint8_t pin_cfg_ctrl2;
	/* Default PWM bit count in DPCD register. */
	uint8_t pwm_default;
	/* Current PWM bit count in DPCD register. */
	uint16_t pwm_value;
	/* Default PWM frequency in DPCD register. */
	uint8_t pwm_default_freq;
	/* Panel T3 timing value. */
	uint8_t t3_timing;
	/* Panel T12 timing value. */
	uint8_t t12_timing;
	/* Back light control register. */
	uint8_t backlight_ctrl;
	/* Panel T2 delay. */
	uint8_t t2_delay;
	/* Panel T4 timing value. */
	uint8_t t4_timing;
	/* Panel T5 delay. */
	uint8_t t5_delay;
} __packed;

struct ptn_3460_flash {
	/* Flash command (erase or erase and flash). */
	uint8_t cmd;
	/* Magic number needed by the flash algorithm. */
	uint16_t magic;
	/* Trigger for starting flash operation. */
	uint8_t trigger;
} __packed;

int ptn3460_init(const char *hwi_block);
int ptn3460_write_edid(uint8_t edid_num, const uint8_t data[PTN_EDID_LEN]);
int ptn_select_edid(uint8_t edid_num);
#endif /* PTN3460_H_ */
