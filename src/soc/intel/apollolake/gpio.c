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
#include <soc/itss.h>
#include <soc/pm.h>

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

static void gpio_configure_itss(const struct pad_config *cfg,
				uint16_t port, uint16_t pad_cfg_offset)
{
	int irq;

	/* Set up ITSS polarity if pad is routed to APIC.
	 *
	 * The ITSS takes only active high interrupt signals. Therefore,
	 * if the pad configuration indicates an inversion assume the
	 * intent is for the ITSS polarity. Before forwarding on the
	 * request to the APIC there's an inversion setting for how the
	 * signal is forwarded to the APIC. Honor the inversion setting
	 * in the GPIO pad configuration so that a hardware active low
	 * signal looks that way to the APIC (double inversion).
	 */
	if (!(cfg->config0 & PAD_CFG0_ROUTE_IOAPIC))
		return;

	irq = iosf_read(port, pad_cfg_offset + sizeof(uint32_t));
	irq &= PAD_CFG1_IRQ_MASK;
	if (!irq) {
		printk(BIOS_ERR, "GPIO %u doesn't support APIC routing,\n",
			cfg->pad);
		return;
	}

	itss_set_irq_polarity(irq, !!(cfg->config0 & PAD_CFG0_RX_POL_INVERT));
}

void gpio_configure_pad(const struct pad_config *cfg)
{
	uint32_t dw1;
	const struct pad_community *comm = gpio_get_community(cfg->pad);
	uint16_t config_offset = PAD_CFG_OFFSET(cfg->pad - comm->first_pad);

	/* Iostandby bits are tentatively stored in [3:0] bits (RO) of config1.
	 * dw1 is used to extract the bits of Iostandby.
	 * This is done to preserve config1 size as unit16 in gpio.h.
	 */
	dw1 = cfg->config1 & ~PAD_CFG1_IOSSTATE_MASK;
	dw1 |= (cfg->config1 & PAD_CFG1_IOSSTATE_MASK) << PAD_CFG1_IOSSTATE_SHIFT;

	iosf_write(comm->port, config_offset, cfg->config0);
	iosf_write(comm->port, config_offset + sizeof(uint32_t), dw1);

	gpio_configure_itss(cfg, comm->port, config_offset);
}

void gpio_configure_pads(const struct pad_config *cfg, size_t num_pads)
{
	uint32_t i;

	for (i = 0; i < num_pads; i++)
		gpio_configure_pad(cfg + i);
}

void gpio_input_pulldown(gpio_t gpio)
{
	struct pad_config cfg = PAD_CFG_GPI(gpio, DN_20K, DEEP);
	gpio_configure_pad(&cfg);
}

void gpio_input_pullup(gpio_t gpio)
{
	struct pad_config cfg = PAD_CFG_GPI(gpio, UP_20K, DEEP);
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

uint16_t gpio_acpi_pin(gpio_t gpio_num)
{
	const struct pad_community *comm = gpio_get_community(gpio_num);

	switch (comm->port) {
	case GPIO_NORTH:
		return PAD_N(gpio_num);
	case GPIO_NORTHWEST:
		return PAD_NW(gpio_num);
	case GPIO_WEST:
		return PAD_W(gpio_num);
	case GPIO_SOUTHWEST:
		return PAD_SW(gpio_num);
	}

	return gpio_num;
}

/* Helper function to map PMC register groups to tier1 sci groups */
static int pmc_gpe_route_to_gpio(int route)
{
	switch(route) {
	case PMC_GPE_SW_31_0:
		return GPIO_GPE_SW_31_0;
	case PMC_GPE_SW_63_32:
		return GPIO_GPE_SW_63_32;
	case PMC_GPE_NW_31_0:
		return GPIO_GPE_NW_31_0;
	case PMC_GPE_NW_63_32:
		return GPIO_GPE_NW_63_32;
	case PMC_GPE_NW_95_64:
		return GPIO_GPE_NW_95_64;
	case PMC_GPE_N_31_0:
		return GPIO_GPE_N_31_0;
	case PMC_GPE_N_63_32:
		return GPIO_GPE_N_63_32;
	case PMC_GPE_W_31_0:
		return GPIO_GPE_W_31_0;
	default:
		return -1;
	}
}

void gpio_route_gpe(uint8_t gpe0b, uint8_t gpe0c, uint8_t gpe0d)
{
	int i;
	uint32_t misccfg_mask;
	uint32_t misccfg_value;
	uint32_t value;

	/* Get the group here for community specific MISCCFG register.
	 * If any of these returns -1 then there is some error in devicetree
	 * where the group is probably hardcoded and does not comply with the
	 * PMC group defines. So we return from here and MISCFG is set to
	 * default.
	 */
	gpe0b = pmc_gpe_route_to_gpio(gpe0b);
	if(gpe0b == -1)
		return;
	gpe0c = pmc_gpe_route_to_gpio(gpe0c);
	if(gpe0c == -1)
		return;
	gpe0d = pmc_gpe_route_to_gpio(gpe0d);
	if(gpe0d == -1)
		return;

	misccfg_value = gpe0b << MISCCFG_GPE0_DW0_SHIFT;
	misccfg_value |= gpe0c << MISCCFG_GPE0_DW1_SHIFT;
	misccfg_value |= gpe0d << MISCCFG_GPE0_DW2_SHIFT;

	/* Program GPIO_MISCCFG */
	misccfg_mask = ~(MISCCFG_GPE0_DW2_MASK |
			MISCCFG_GPE0_DW1_MASK |
			MISCCFG_GPE0_DW0_MASK);

	for (i = 0; i < ARRAY_SIZE(gpio_communities); i++) {
		const struct pad_community *comm = &gpio_communities[i];

		value = iosf_read(comm->port, GPIO_MISCCFG);
		value &= misccfg_mask;
		value |= misccfg_value;
		iosf_write(comm->port, GPIO_MISCCFG, value);
	}
}
