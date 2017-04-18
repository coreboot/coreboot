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

enum gpio_irq_type {
	IRQ_TYPE_EDGE_RISING,
	IRQ_TYPE_EDGE_FALLING,
	IRQ_TYPE_LEVEL_HIGH,
	IRQ_TYPE_LEVEL_LOW,
};

struct eint_section {
	uint32_t	regs[7];
	uint32_t	align1[9];
};

struct eint_regs {
	struct eint_section sta;
	struct eint_section ack;
	struct eint_section mask;
	struct eint_section mask_set;
	struct eint_section mask_clr;
	struct eint_section sens;
	struct eint_section sens_set;
	struct eint_section sens_clr;
	struct eint_section soft;
	struct eint_section soft_set;
	struct eint_section soft_clr;
	struct eint_section rsv00;
	struct eint_section pol;
	struct eint_section pol_set;
	struct eint_section pol_clr;
	struct eint_section rsv01;
	uint32_t	    d0en[7];
	uint32_t	    rsv02;
	uint32_t	    d1en[7];
};

check_member(eint_regs, d1en, 0x420);

static struct eint_regs *const mt8173_eint = (void *)(EINT_BASE);

/*
 * Firmware never enables interrupts on this platform.  This function
 * reads current EINT status and clears the pending interrupt.
 *
 * Returns 1 if the interrupt was pending, else 0.
 */
int gpio_eint_poll(gpio_t gpio);

/*
 * Configure a GPIO to handle external interrupts (EINT) of given irq type.
 */
void gpio_eint_configure(gpio_t gpio, enum gpio_irq_type type);

#endif /* SOC_MEDIATEK_MT8173_GPIO_H */
