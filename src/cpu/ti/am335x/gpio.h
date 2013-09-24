/*
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CPU_TI_AM335X_GPIO_H__
#define __CPU_TI_AM335X_GPIO_H__

#include <stdint.h>

enum {
	AM335X_GPIO_BITS_PER_BANK = 32
};

struct am335x_gpio_regs {
	uint32_t revision;		// 0x0
	uint8_t _rsv0[0xc];		// 0x4-0xf
	uint32_t sysconfig;		// 0x10
	uint8_t _rsv1[0xc];		// 0x14-0x1f
	uint32_t eoi;			// 0x20
	uint32_t irqstatus_raw_0;	// 0x24
	uint32_t irqstatus_raw_1;	// 0x28
	uint32_t irqstatus_0;		// 0x2c
	uint32_t irqstatus_1;		// 0x30
	uint32_t irqstatus_set_0;	// 0x34
	uint32_t irqstatus_set_1;	// 0x38
	uint32_t irqstatus_clr_0;	// 0x3c
	uint32_t irqstatus_clk_1;	// 0x40
	uint32_t irqwaken_0;		// 0x44
	uint32_t irqwaken_1;		// 0x48
	uint8_t _rsv2[0xc8];		// 0x4c-0x113
	uint32_t sysstatus;		// 0x114
	uint8_t _rsv3[0x18];		// 0x118-0x12f
	uint32_t ctrl;			// 0x130
	uint32_t oe;			// 0x134
	uint32_t datain;		// 0x138
	uint32_t dataout;		// 0x13c
	uint32_t leveldetect0;		// 0x140
	uint32_t leveldetect1;		// 0x144
	uint32_t risingdetect;		// 0x148
	uint32_t fallingdetect;		// 0x14c
	uint32_t debouncenable;		// 0x150
	uint32_t debouncingtime;	// 0x154
	uint8_t _rsv4[0x38];		// 0x158-0x18f
	uint32_t cleardataout;		// 0x190
	uint32_t setdataout;		// 0x194
} __attribute__((packed));

static struct am335x_gpio_regs * const am335x_gpio_banks[] = {
	(void *)0x44e07000, (void *)0x4804c000,
	(void *)0x481ac000, (void *)0x481ae000
};

void am335x_disable_gpio_irqs(void);

int gpio_direction_input(unsigned gpio);
int gpio_direction_output(unsigned gpio, int value);
int gpio_get_value(unsigned gpio);
int gpio_set_value(unsigned gpio, int value);

#endif	/* __CPU_TI_AM335X_CLOCK_H__ */
