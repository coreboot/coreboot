/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018       Facebook, Inc.
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

#include <console/console.h>
#include <soc/gpio.h>
#include <arch/io.h>
#include <endian.h>
#include <soc/addressmap.h>

union gpio_const {
	u64 u;
	struct {
		u64 gpios:8; /** Number of GPIOs implemented */
		u64 pp:8; /** Number of PP vectors */
		u64:48; /* Reserved */
	} s;
};
union bit_cfg {
	u64 u;
	struct {
		u64 tx_oe	: 1; /* Output Enable */
		u64 xor		: 1; /* Invert */
		u64 int_en	: 1; /* Interrupt Enable */
		u64 int_type	: 1; /* Type of Interrupt */
		u64 filt_cnt	: 4; /* Glitch filter counter */
		u64 filt_sel	: 4; /* Glitch filter select */
		u64 tx_od	: 1; /* Set Output to Open Drain */
		u64 		: 3;
		u64 pin_sel	: 10; /* Select type of pin */
		u64 		: 38;
	} s;
};

struct cavium_gpio {
	u64 rx_dat;
	u64 tx_set;
	u64 tx_clr;
	u64 multicast;
	u64 ocla_exten_trg;
	u64 strap;
	u64 reserved[12];
	union gpio_const gpio_const; /* Offset 90 */
	u64 reserved2[109];
	union bit_cfg bit_cfg[48]; /* Offset 400 */
};

/* Base address of GPIO BAR */
static const void *gpio_get_baseaddr(void)
{
	return (const void *)GPIO_PF_BAR0;
}

/* Number of GPIO pins. Usually 48. */
gpio_t gpio_pin_count(void)
{
	struct cavium_gpio *regs = (struct cavium_gpio *)gpio_get_baseaddr();
	union gpio_const gpio_const;

	gpio_const.u = read64(&regs->gpio_const.u);

	if (gpio_const.s.gpios > 64)
		return 64; // FIXME: Add support for more than 64 GPIOs
	return gpio_const.s.gpios;
}

/* Set GPIO to software control and direction INPUT */
void gpio_input(gpio_t gpio)
{
	struct cavium_gpio *regs = (struct cavium_gpio *)gpio_get_baseaddr();
	union bit_cfg bit_cfg;

	if (gpio >= gpio_pin_count())
		return;

	printk(BIOS_SPEW, "GPIO(%u): direction input\n", gpio);

	bit_cfg.u = read64(&regs->bit_cfg[gpio]);
	bit_cfg.s.pin_sel = 0;
	bit_cfg.s.tx_oe = 0;
	write64(&regs->bit_cfg[gpio], bit_cfg.u);
}

/* Set GPIO of direction OUTPUT to level */
void gpio_set(gpio_t gpio, int value)
{
	struct cavium_gpio *regs = (struct cavium_gpio *)gpio_get_baseaddr();

	if (gpio >= gpio_pin_count())
		return;

	printk(BIOS_SPEW, "GPIO(%u): level: %u\n", gpio, !!value);

	if (value)
		write64(&regs->tx_set, 1ULL << gpio);
	else
		write64(&regs->tx_clr, 1ULL << gpio);
}

/* Set GPIO direction to OUTPUT with level */
void gpio_output(gpio_t gpio, int value)
{
	struct cavium_gpio *regs = (struct cavium_gpio *)gpio_get_baseaddr();
	union bit_cfg bit_cfg;

	if (gpio >= gpio_pin_count())
		return;

	gpio_set(gpio, value);

	printk(BIOS_SPEW, "GPIO(%u): direction output with level: %u\n", gpio,
	       !!value);

	bit_cfg.u = read64(&regs->bit_cfg[gpio]);
	bit_cfg.s.pin_sel = 0;
	bit_cfg.s.tx_oe = 1;
	write64(&regs->bit_cfg[gpio], bit_cfg.u);
}

/* Set GPIO invert flag, that affects INPUT and OUTPUT */
void gpio_invert(gpio_t gpio, int value)
{
	struct cavium_gpio *regs = (struct cavium_gpio *)gpio_get_baseaddr();
	union bit_cfg bit_cfg;

	if (gpio >= gpio_pin_count())
		return;

	bit_cfg.u = read64(&regs->bit_cfg[gpio]);
	bit_cfg.s.xor = !!value;
	write64(&regs->bit_cfg[gpio], bit_cfg.u);

	printk(BIOS_SPEW, "GPIO(%u): invert: %s\n", gpio, value ? "ON" : "OFF");
}

/* Read GPIO level with direction set to INPUT */
int gpio_get(gpio_t gpio)
{
	struct cavium_gpio *regs = (struct cavium_gpio *)gpio_get_baseaddr();

	if (gpio >= gpio_pin_count())
		return 0;

	const u64 reg = read64(&regs->rx_dat);
	printk(BIOS_SPEW, "GPIO(%u): input: %u\n", gpio,
	       !!(reg & (1ULL << gpio)));

	return !!(reg & (1ULL << gpio));
}

/* Read GPIO STRAP level sampled at cold boot */
int gpio_strap_value(gpio_t gpio)
{
	struct cavium_gpio *regs = (struct cavium_gpio *)gpio_get_baseaddr();

	if (gpio >= gpio_pin_count())
		return 0;

	const u64 reg = read64(&regs->strap);
	printk(BIOS_SPEW, "GPIO(%u): strap: %u\n", gpio,
	       !!(reg & (1ULL << gpio)));

	return !!(reg & (1ULL << gpio));
}

/* FIXME: Parse devicetree ? */
void gpio_init(void)
{
	const size_t pin_count = gpio_pin_count();

	printk(BIOS_DEBUG, "GPIO: base address: %p, pin count: %zd\n",
	       gpio_get_baseaddr(), pin_count);

	if (!pin_count)
		return;
}

void gpio_input_pulldown(gpio_t gpio)
{
}

void gpio_input_pullup(gpio_t gpio)
{
}
