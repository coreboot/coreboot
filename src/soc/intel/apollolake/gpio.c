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
	uint8_t num_gpi_regs;
	uint8_t gpi_offset;
	const char *grp_name;
} gpio_communities[] = {
	{
		.port = GPIO_SW,
		.first_pad = SW_OFFSET,
		.num_gpi_regs = NUM_SW_GPI_REGS,
		.gpi_offset = 0,
		.grp_name = "GPIO_GPE_SW",
	}, {
		.port = GPIO_W,
		.first_pad = W_OFFSET,
		.num_gpi_regs = NUM_W_GPI_REGS,
		.gpi_offset = NUM_SW_GPI_REGS,
		.grp_name = "GPIO_GPE_W",
	}, {
		.port = GPIO_NW,
		.first_pad = NW_OFFSET,
		.num_gpi_regs = NUM_NW_GPI_REGS,
		.gpi_offset = NUM_W_GPI_REGS + NUM_SW_GPI_REGS,
		.grp_name = "GPIO_GPE_NW",
	}, {
		.port = GPIO_N,
		.first_pad = N_OFFSET,
		.num_gpi_regs = NUM_N_GPI_REGS,
		.gpi_offset = NUM_NW_GPI_REGS+ NUM_W_GPI_REGS + NUM_SW_GPI_REGS,
		.grp_name = "GPIO_GPE_N",
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
	/* No ITSS configuration in SMM. */
	if (ENV_SMM)
		return;

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

static void gpio_configure_owner(const struct pad_config *cfg,
				 uint16_t port, int pin)
{
	uint32_t val;
	uint16_t hostsw_reg;

	/* The 4th bit in pad_config 1 (RO) is used to indicate if the pad
	 * needs GPIO driver ownership.
	 */
	if (!(cfg->config1 & PAD_CFG1_GPIO_DRIVER))
		return;

	/* Based on the gpio pin number configure the corresponding bit in
	 * HOSTSW_OWN register. Value of 0x1 indicates GPIO Driver onwership.
	 */
	hostsw_reg = HOSTSW_OWN_REG_BASE + ((pin / 32) * sizeof(uint32_t));
	val = iosf_read(port, hostsw_reg);
	val |= 1 << (pin % 32);
	iosf_write(port, hostsw_reg, val);
}

static void gpi_enable_smi(const struct pad_config *cfg, uint16_t port, int pin)
{
	uint32_t value;
	uint16_t sts_reg;
	uint16_t en_reg;
	int group;

	if (((cfg->config0) & PAD_CFG0_ROUTE_SMI) != PAD_CFG0_ROUTE_SMI)
		return;

	group = pin / GPIO_MAX_NUM_PER_GROUP;

	sts_reg = GPI_SMI_STS_OFFSET(group);
	value = iosf_read(port, sts_reg);
	/* Write back 1 to reset the sts bits */
	iosf_write(port, sts_reg, value);

	/* Set enable bits */
	en_reg = GPI_SMI_EN_OFFSET(group);
	value = iosf_read(port, en_reg );
	value |= 1 << (pin % GPIO_MAX_NUM_PER_GROUP);
	iosf_write(port, en_reg , value);
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
	gpio_configure_owner(cfg, comm->port, cfg->pad - comm->first_pad);

	gpi_enable_smi(cfg, comm->port, cfg->pad - comm->first_pad);
}

void gpio_configure_pads(const struct pad_config *cfg, size_t num_pads)
{
	uint32_t i;

	for (i = 0; i < num_pads; i++)
		gpio_configure_pad(cfg + i);
}

void *gpio_dwx_address(const uint16_t pad)
{
	/* Calculate Address of DW0 register for given GPIO
	 * pad - GPIO number
	 * returns - address of GPIO
	 */
	const struct pad_community *comm = gpio_get_community(pad);
	return iosf_address(comm->port, PAD_CFG_OFFSET(pad - comm->first_pad));
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
	case GPIO_N:
		return "\\_SB.GPO0";
	case GPIO_NW:
		return "\\_SB.GPO1";
	case GPIO_W:
		return "\\_SB.GPO2";
	case GPIO_SW:
		return "\\_SB.GPO3";
	}

	return NULL;
}

uint16_t gpio_acpi_pin(gpio_t gpio_num)
{
	const struct pad_community *comm = gpio_get_community(gpio_num);

	switch (comm->port) {
	case GPIO_N:
		return PAD_N(gpio_num);
	case GPIO_NW:
		return PAD_NW(gpio_num);
	case GPIO_W:
		return PAD_W(gpio_num);
	case GPIO_SW:
		return PAD_SW(gpio_num);
	}

	return gpio_num;
}

static void print_gpi_status(const struct gpi_status *sts)
{
	int i;
	int group;
	int index = 0;
	int bit_set;
	int num_groups;
	int abs_bit;
	const struct pad_community *comm;

	for (i = 0; i < ARRAY_SIZE(gpio_communities); i++) {
		comm = &gpio_communities[i];
		num_groups = comm->num_gpi_regs;
		index = comm->gpi_offset;
		for (group = 0; group < num_groups; group++, index++) {
			for (bit_set = 31; bit_set >= 0; bit_set--) {
				if (!(sts->grp[index] & (1 << bit_set)))
					continue;

				abs_bit = bit_set;
				abs_bit += group * GPIO_MAX_NUM_PER_GROUP;
				printk(BIOS_DEBUG, "%s %d\n",comm->grp_name,
								abs_bit);
			}
		}
	}
}

void gpi_clear_get_smi_status(struct gpi_status *sts)
{
	int i;
	int group;
	int index = 0;
	uint32_t sts_value;
	uint32_t en_value;
	int num_groups;
	const struct pad_community *comm;

	for (i = 0; i < ARRAY_SIZE(gpio_communities); i++) {
		comm = &gpio_communities[i];
		num_groups = comm->num_gpi_regs;
		index = comm->gpi_offset;
		for (group = 0; group < num_groups; group++, index++) {
			sts_value = iosf_read(gpio_communities[i].port,
					GPI_SMI_STS_OFFSET(group));
			en_value = iosf_read(gpio_communities[i].port,
					GPI_SMI_EN_OFFSET(group));
			sts->grp[index] = sts_value & en_value;
			/* Clear the set status bits. */
			iosf_write(gpio_communities[i].port,
				GPI_SMI_STS_OFFSET(group), sts->grp[index]);
		}
	}

	if (IS_ENABLED(CONFIG_DEBUG_SMI))
		print_gpi_status(sts);

}

int gpi_status_get(const struct gpi_status *sts, gpio_t gpi)
{
	uint8_t sts_index;
	const struct pad_community *comm = gpio_get_community(gpi);

	/* Check if valid gpi */
	if (comm == NULL)
		return 0;

	sts_index = comm->gpi_offset + ((gpi - comm->first_pad) /
					GPIO_MAX_NUM_PER_GROUP);

	return !!(sts->grp[sts_index] & (1 << (gpi % GPIO_MAX_NUM_PER_GROUP)));
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
	int ret;

	/* Get the group here for community specific MISCCFG register.
	 * If any of these returns -1 then there is some error in devicetree
	 * where the group is probably hardcoded and does not comply with the
	 * PMC group defines. So we return from here and MISCFG is set to
	 * default.
	 */
	ret = pmc_gpe_route_to_gpio(gpe0b);
	if (ret == -1)
		return;
	gpe0b = ret;

	ret = pmc_gpe_route_to_gpio(gpe0c);
	if (ret == -1)
		return;
	gpe0c = ret;

	ret = pmc_gpe_route_to_gpio(gpe0d);
	if (ret == -1)
		return;
	gpe0d = ret;

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
