/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <assert.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/iosf.h>

/* This list must be in order, from highest pad numbers, to lowest pad numbers*/
static const struct pad_community {
	uint16_t first_pad;
	uint8_t port;
} gpio_communities[] = {
	{
		.port = GPIO_SOUTHWEST,
		.first_pad = SW_OFFSET,
	}, {
		.port = GPIO_WEST,
		.first_pad = W_OFFSET,
	}, {
		.port = GPIO_NORTHWEST,
		.first_pad = NW_OFFSET,
	}, {
		.port = GPIO_NORTH,
		.first_pad = N_OFFSET,
	}
};

static const struct pad_community *gpio_get_community(uint16_t pad)
{
	const struct pad_community *map = gpio_communities;

	assert(pad < TOTAL_PADS);

	while (map->first_pad > pad)
		map++;

	return map;
}

void gpio_configure_pad(const struct pad_config *cfg)
{
	const struct pad_community *comm = gpio_get_community(cfg->pad);
	uint16_t config_offset = PAD_CFG_OFFSET(cfg->pad - comm->first_pad);
	iosf_write(comm->port, config_offset, cfg->config0);
	iosf_write(comm->port, config_offset + sizeof(uint32_t), cfg->config1);
}

void gpio_configure_pads(const struct pad_config *cfg, size_t num_pads)
{
	uint32_t i;

	for (i = 0; i < num_pads; i++)
		gpio_configure_pad(cfg + i);
}

void gpio_input_pulldown(gpio_t gpio)
{
	struct pad_config cfg = PAD_CFG_GPI(gpio, DN_5K, DEEP);
	gpio_configure_pad(&cfg);
}

void gpio_input_pullup(gpio_t gpio)
{
	struct pad_config cfg = PAD_CFG_GPI(gpio, UP_5K, DEEP);
	gpio_configure_pad(&cfg);
}

void gpio_input(gpio_t gpio)
{
	struct pad_config cfg = PAD_CFG_GPI(gpio, NONE, DEEP);
	gpio_configure_pad(&cfg);
}

void gpio_output(gpio_t gpio, int value)
{
	struct pad_config cfg = PAD_CFG_GPO(gpio, value, DEEP);
	gpio_configure_pad(&cfg);
}

int gpio_get(gpio_t gpio_num)
{
	uint32_t reg;
	const struct pad_community *comm = gpio_get_community(gpio_num);
	uint16_t config_offset = PAD_CFG_OFFSET(gpio_num - comm->first_pad);

	reg = iosf_read(comm->port, config_offset);

	return !!(reg & PAD_CFG0_RX_STATE);
}

void gpio_set(gpio_t gpio_num, int value)
{
	uint32_t reg;
	const struct pad_community *comm = gpio_get_community(gpio_num);
	uint16_t config_offset = PAD_CFG_OFFSET(gpio_num - comm->first_pad);

	reg = iosf_read(comm->port, config_offset);
	reg &= ~PAD_CFG0_TX_STATE;
	reg |= !!value & PAD_CFG0_TX_STATE;
	iosf_write(comm->port, config_offset, reg);
}

const char *gpio_acpi_path(gpio_t gpio_num)
{
	const struct pad_community *comm = gpio_get_community(gpio_num);

	switch (comm->port) {
	case GPIO_NORTH:
		return "\\_SB.GPO0";
	case GPIO_NORTHWEST:
		return "\\_SB.GPO1";
	case GPIO_WEST:
		return "\\_SB.GPO2";
	case GPIO_SOUTHWEST:
		return "\\_SB.GPO3";
	}

	return NULL;
}
