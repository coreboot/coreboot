/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <arch/io.h>
#include <soc/addressmap.h>
#include <soc/padconfig.h>

static uint32_t * const pinmux_regs = (void *)(uintptr_t)TEGRA_APB_PINMUX_BASE;
static struct gpio_bank * const gpio_regs = (void *)(uintptr_t)TEGRA_GPIO_BASE;

static inline struct gpio_bank * const get_gpio_bank_regs(int index)
{
	return &gpio_regs[gpio_index_to_bank(index)];
}

static inline uint32_t pad_get_pinmux(int index)
{
	return read32(&pinmux_regs[index]);
}

static inline void pad_set_pinmux(int index, uint32_t reg)
{
	return write32(&pinmux_regs[index], reg);
}

static inline void pad_set_gpio_out(int gpio_index, int val)
{
	struct gpio_bank * const regs = get_gpio_bank_regs(gpio_index);
	int port = gpio_index_to_port(gpio_index);
	int bit = gpio_to_bit(gpio_index);

	write32(&regs->out_value_mask[port],
		(1 << (bit + GPIO_GPIOS_PER_PORT)) | (val << bit));
	write32(&regs->out_enable_mask[port],
		(1 << (bit + GPIO_GPIOS_PER_PORT)) | (1 << bit));
}

static inline void pad_set_mode(int gpio_index, int sfio_or_gpio)
{
	struct gpio_bank * const regs = get_gpio_bank_regs(gpio_index);
	int port = gpio_index_to_port(gpio_index);
	int bit = gpio_to_bit(gpio_index);

	write32(&regs->config_mask[port],
		(1 << (bit + GPIO_GPIOS_PER_PORT)) | (sfio_or_gpio << bit));
}

static inline void pad_set_gpio_mode(int gpio_index)
{
	pad_set_mode(gpio_index, 1);
}

static inline void pad_set_sfio_mode(int gpio_index)
{
	pad_set_mode(gpio_index, 0);
}

static void configure_unused_pad(const struct pad_config * const entry)
{
	uint32_t reg;

	/*
	 * Tristate the pad and disable input. If power-on-reset state is a
	 * pullup maintain that. Otherwise enable pulldown.
	 */
	reg = pad_get_pinmux(entry->pinmux_index);
	reg &= ~PINMUX_INPUT_ENABLE;
	reg |= PINMUX_TRISTATE;
	reg &= ~PINMUX_PULL_MASK;
	if (entry->por_pullup)
		reg |= PINMUX_PULL_UP;
	else
		reg |= PINMUX_PULL_DOWN;
	pad_set_pinmux(entry->pinmux_index, reg);

	/*
	 * Set to GPIO mode if GPIO available to bypass collisions of
	 * controller signals going to more than one pad.
	 */
	if (entry->pad_has_gpio)
		pad_set_gpio_mode(entry->gpio_index);
}

static void configure_sfio_pad(const struct pad_config * const entry)
{
	pad_set_pinmux(entry->pinmux_index, entry->pinmux_flags);
	pad_set_sfio_mode(entry->gpio_index);
}

static void configure_gpio_pad(const struct pad_config * const entry)
{
	uint32_t reg;

	if (entry->gpio_out0 || entry->gpio_out1)
		pad_set_gpio_out(entry->gpio_index, entry->gpio_out1 ? 1 : 0);

	/* Keep the original SFIO selection. */
	reg = pinmux_get_config(entry->pinmux_index);
	reg &= PINMUX_FUNC_MASK;
	reg |= entry->pinmux_flags;

	pad_set_pinmux(entry->pinmux_index, reg);
	pad_set_gpio_mode(entry->gpio_index);
}

void soc_configure_pads(const struct pad_config * const entries, size_t num)
{
	size_t i;

	for (i = 0; i < num; i++) {
		const struct pad_config * const entry = &entries[i];

		if (entry->unused) {
			configure_unused_pad(entry);
		} else if (entry->sfio) {
			configure_sfio_pad(entry);
		} else {
			configure_gpio_pad(entry);
		}
	}
}
