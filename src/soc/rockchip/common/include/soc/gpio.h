/*
 * This file is part of the coreboot project.
 *
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

#ifndef __COREBOOT_SRC_SOC_ROCKCHIP_COMMON_INCLUDE_SOC_GPIO_H
#define __COREBOOT_SRC_SOC_ROCKCHIP_COMMON_INCLUDE_SOC_GPIO_H

#include <types.h>

#define GPIO(p, b, i) ((gpio_t){.port = p, .bank = GPIO_##b, .idx = i})

struct rockchip_gpio_regs {
	u32 swporta_dr;
	u32 swporta_ddr;
	u32 reserved0[(0x30 - 0x08) / 4];
	u32 inten;
	u32 intmask;
	u32 inttype_level;
	u32 int_polarity;
	u32 int_status;
	u32 int_rawstatus;
	u32 debounce;
	u32 porta_eoi;
	u32 ext_porta;
	u32 reserved1[(0x60 - 0x54) / 4];
	u32 ls_sync;
};
check_member(rockchip_gpio_regs, ls_sync, 0x60);

typedef union {
	u32 raw;
	struct {
		union {
			struct {
				u32 num : 5;
				u32 reserved1 : 27;
			};
			struct {
				u32 idx : 3;
				u32 bank : 2;
				u32 port : 4;
				u32 reserved2 : 23;
			};
		};
	};
} gpio_t;

enum {
	GPIO_A = 0,
	GPIO_B,
	GPIO_C,
	GPIO_D,
};

extern struct rockchip_gpio_regs *gpio_port[];

/* Check if the gpio port is a pmu gpio */
int is_pmu_gpio(gpio_t gpio);

/* Return the io addr of gpio register */
void *gpio_grf_reg(gpio_t gpio);

enum gpio_pull {
	GPIO_PULLNONE = 0,
	GPIO_PULLUP = 1,
	GPIO_PULLDOWN = 2,
};

enum gpio_dir {
	GPIO_INPUT = 0,
	GPIO_OUTPUT = 1,
};

enum gpio_irq_type {
	IRQ_TYPE_EDGE_RISING = 0,
	IRQ_TYPE_EDGE_FALLING,
	IRQ_TYPE_LEVEL_HIGH,
	IRQ_TYPE_LEVEL_LOW,
};

/* Setup and enable irq */
void gpio_input_irq(gpio_t gpio, enum gpio_irq_type type, enum gpio_pull pull);

/* Check and clear irq status */
int gpio_irq_status(gpio_t gpio);

/* The gpio pull bias setting may be different between SoCs */
u32 gpio_get_pull_val(gpio_t gpio, enum gpio_pull pull);

#endif
