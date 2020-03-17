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

#ifndef SOC_MEDIATEK_COMMON_GPIO_H
#define SOC_MEDIATEK_COMMON_GPIO_H

#include <soc/gpio_base.h>
#include <stddef.h>
#include <stdint.h>

enum pull_enable {
	GPIO_PULL_DISABLE = 0,
	GPIO_PULL_ENABLE = 1,
};

enum pull_select {
	GPIO_PULL_DOWN = 0,
	GPIO_PULL_UP = 1,
};

void gpio_set_pull(gpio_t gpio, enum pull_enable enable,
		   enum pull_select select);
void gpio_set_mode(gpio_t gpio, int mode);

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

static struct eint_regs *const mtk_eint = (void *)(EINT_BASE);

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

#endif
