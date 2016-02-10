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
 */

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
		.first_pad = 0,
	}
};

static const struct pad_community *gpio_get_community(uint16_t pad)
{
	const struct pad_community *map = gpio_communities;

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
