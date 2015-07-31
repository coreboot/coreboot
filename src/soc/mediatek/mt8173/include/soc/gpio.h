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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#ifndef SOC_MEDIATEK_MT8173_GPIO_H
#define SOC_MEDIATEK_MT8173_GPIO_H

#include <stdint.h>
#include <stdlib.h>
#include <soc/addressmap.h>

enum pull_enable {
	GPIO_PULL_DISABLE = 0,
	GPIO_PULL_ENABLE = 1,
};

enum pull_select {
	GPIO_PULL_DOWN = 0,
	GPIO_PULL_UP = 1,
};

enum external_power {
	GPIO_EINT_3P3V = 0,
	GPIO_EINT_1P8V = 1,
};

typedef u32 gpio_t;

struct val_regs {
	uint16_t val;
	uint16_t align1;
	uint16_t set;
	uint16_t align2;
	uint16_t rst;
	uint16_t align3[3];
};

struct gpio_regs {
	struct val_regs dir[9];
	uint8_t rsv00[112];
	struct val_regs pullen[9];
	uint8_t rsv01[112];
	struct val_regs pullsel[9];
	uint8_t rsv02[112];
	uint8_t rsv03[256];
	struct val_regs dout[9];
	uint8_t rsv04[112];
	struct val_regs din[9];
	uint8_t rsv05[112];
	struct val_regs mode[27];
	uint8_t rsv06[336];
	struct val_regs ies[3];
	struct val_regs smt[3];
	uint8_t rsv07[160];
	struct val_regs tdsel[8];
	struct val_regs rdsel[6];
	uint8_t rsv08[32];
	struct val_regs drv_mode[10];
	uint8_t rsv09[96];
	struct val_regs msdc_rsv0[11];
	struct val_regs msdc2_ctrl5;
	struct val_regs msdc_rsv1[12];
	uint8_t rsv10[64];
	struct val_regs exmd_ctrl[1];
	uint8_t rsv11[48];
	struct val_regs kpad_ctrl[2];
	struct val_regs hsic_ctrl[4];
};

check_member(gpio_regs, msdc2_ctrl5, 0xcb0);
check_member(gpio_regs, hsic_ctrl[3], 0xe50);

static struct gpio_regs *const mt8173_gpio = (void *)(GPIO_BASE);

void gpio_set_pull(gpio_t gpio, enum pull_enable enable,
		   enum pull_select select);
void gpio_set_mode(gpio_t gpio, int mode);
void gpio_init(enum external_power);

#endif /* SOC_MEDIATEK_MT8173_GPIO_H */
