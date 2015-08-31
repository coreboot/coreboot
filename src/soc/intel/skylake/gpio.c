/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#include <stdint.h>
#include <string.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <gpio.h>
#include <soc/pcr.h>
#include <soc/iomap.h>
#include <soc/pm.h>

static const int gpio_debug = 0;

/* There are 4 communities with 8 GPIO groups (GPP_[A:G] and GPD) */
struct gpio_community {
	int port_id;
	/* Inclusive pads within the community. */
	gpio_t min;
	gpio_t max;
};

/* This is ordered to match ACPI and OS driver. */
static const struct gpio_community communities[] = {
	{
		.port_id = PID_GPIOCOM0,
		.min = GPP_A0,
		.max = GPP_B23,
	},
	{
		.port_id = PID_GPIOCOM1,
		.min = GPP_C0,
		.max = GPP_E23,
	},
	{
		.port_id = PID_GPIOCOM3,
		.min = GPP_F0,
		.max = GPP_G7,
	},
	{
		.port_id = PID_GPIOCOM2,
		.min = GPD0,
		.max = GPD11,
	},
};

static const char *gpio_group_names[GPIO_NUM_GROUPS] = {
	"GPP_A",
	"GPP_B",
	"GPP_C",
	"GPP_D",
	"GPP_E",
	"GPP_F",
	"GPP_G",
	"GPD",
};

static inline size_t gpios_in_community(const struct gpio_community *comm)
{
	/* max is inclusive */
	return comm->max - comm->min + 1;
}

static inline size_t groups_in_community(const struct gpio_community *comm)
{
	size_t n = gpios_in_community(comm) + GPIO_MAX_NUM_PER_GROUP - 1;
	return n / GPIO_MAX_NUM_PER_GROUP;
}

static inline int gpio_index_gpd(gpio_t gpio)
{
	if (gpio >= GPD0 && gpio <= GPD11)
		return 1;
	return 0;
}

static const struct gpio_community *gpio_get_community(gpio_t pad)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(communities); i++) {
		const struct gpio_community *c = &communities[i];

		if (pad >= c->min && pad <= c->max)
			return c;
	}

	return NULL;
}

static size_t community_clr_get_smi_sts(const struct gpio_community *comm,
					uint32_t *sts)
{
	uint8_t *regs;
	size_t i;
	uint32_t *gpi_status_reg;
	uint32_t *gpi_en_reg;
	const size_t num_grps = groups_in_community(comm);

	/* Not all groups can be routed to SMI. However, the registers
	 * read as 0. In order to simplify the logic read everything from
	 * each community. */
	regs = pcr_port_regs(comm->port_id);
	gpi_status_reg = (void *)&regs[GPI_SMI_STS_OFFSET];
	gpi_en_reg = (void *)&regs[GPI_SMI_EN_OFFSET];
	for (i = 0; i < num_grps; i++) {
		sts[i] = read32(gpi_status_reg + i) & read32(gpi_en_reg + i);
		/* Clear the enabled and set status bits. */
		write32(gpi_status_reg + i, sts[i]);
	}

	return num_grps;
}

static void print_gpi_status(uint32_t status, const char *grp_name)
{
	int i;

	if (!status)
		return;

	for (i = 31; i >= 0; i--) {
		if (status & (1 << i))
			printk(BIOS_DEBUG, "%s%d ", grp_name, i);
	}
}

void gpi_clear_get_smi_status(struct gpi_status *sts)
{
	int i;
	int do_print;
	size_t sts_index = 0;

	for (i = 0; i < ARRAY_SIZE(communities); i++) {
		const struct gpio_community *comm = &communities[i];
		sts_index += community_clr_get_smi_sts(comm,
						&sts->grp[sts_index]);
	}

	do_print = 0;
	for (i = 0; i < ARRAY_SIZE(sts->grp); i++) {
		if (sts->grp[i] == 0)
			continue;
		do_print = 1;
		break;
	}

	if (!do_print)
		return;

	printk(BIOS_DEBUG, "GPI_SMI_STS: ");
	for (i = 0; i < ARRAY_SIZE(sts->grp); i++)
		print_gpi_status(sts->grp[i], gpio_group_names[i]);
	printk(BIOS_DEBUG, "\n");
}

int gpi_status_get(const struct gpi_status *sts, gpio_t gpi)
{
	const uint32_t *gpi_sts;

	/* Check if valid gpi */
	if (gpio_get_community(gpi) == NULL)
		return 0;

	/* If not in GPD group the index is a linear function based on
	 * GPI number and GPIO_MAX_NUM_PER_GROUP. */
	if (gpio_index_gpd(gpi))
		gpi_sts = &sts->grp[GPD];
	else
		gpi_sts = &sts->grp[gpi / GPIO_MAX_NUM_PER_GROUP];

	return !!(*gpi_sts & (1 << (gpi % GPIO_MAX_NUM_PER_GROUP)));
}

void gpio_route_gpe(uint16_t gpe0_route)
{
	int i;
	uint32_t misc_cfg;
	const uint32_t misc_cfg_reg_mask = GPE_DW_MASK;

	misc_cfg = (uint32_t)gpe0_route << GPE_DW_SHIFT;
	misc_cfg &= misc_cfg_reg_mask;

	for (i = 0; i < ARRAY_SIZE(communities); i++) {
		uint8_t *regs;
		uint32_t reg;
		const struct gpio_community *comm = &communities[i];

		regs = pcr_port_regs(comm->port_id);

		reg = read32(regs + MISCCFG_OFFSET);
		reg &= ~misc_cfg_reg_mask;
		reg |= misc_cfg;
		write32(regs + MISCCFG_OFFSET, reg);
	}
}

static void *gpio_dw_regs(gpio_t pad)
{
	const struct gpio_community *comm;
	uint8_t *regs;
	size_t pad_relative;

	comm = gpio_get_community(pad);

	if (comm == NULL)
		return NULL;

	regs = pcr_port_regs(comm->port_id);

	pad_relative = pad - comm->min;

	/* DW0 and DW1 regs are 4 bytes each. */
	return &regs[PAD_CFG_DW_OFFSET + pad_relative * 8];
}

static void *gpio_hostsw_reg(gpio_t pad, size_t *bit)
{
	const struct gpio_community *comm;
	uint8_t *regs;
	size_t pad_relative;

	comm = gpio_get_community(pad);

	if (comm == NULL)
		return NULL;

	regs = pcr_port_regs(comm->port_id);

	pad_relative = pad - comm->min;

	/* Update the bit for this pad. */
	*bit = (pad_relative % HOSTSW_OWN_PADS_PER);

	/* HostSw regs are 4 bytes each. */
	regs = &regs[HOSTSW_OWN_REG_OFFSET];
	return &regs[(pad_relative / HOSTSW_OWN_PADS_PER) * 4];
}

static void gpio_handle_pad_mode(const struct pad_config *cfg)
{
	size_t bit;
	uint32_t *hostsw_own_reg;
	uint32_t reg;

	bit = 0;
	hostsw_own_reg = gpio_hostsw_reg(cfg->pad, &bit);

	reg = read32(hostsw_own_reg);
	reg &= ~(1U << bit);

	if ((cfg->attrs & PAD_FIELD(HOSTSW, GPIO)) == PAD_FIELD(HOSTSW, GPIO))
		reg |= (HOSTSW_GPIO << bit);
	else
		reg |= (HOSTSW_ACPI << bit);

	write32(hostsw_own_reg, reg);
}

static void gpi_enable_smi(gpio_t pad)
{
	const struct gpio_community *comm;
	uint8_t *regs;
	uint32_t *gpi_status_reg;
	uint32_t *gpi_en_reg;
	size_t group_offset;
	uint32_t pad_mask;

	comm = gpio_get_community(pad);

	regs = pcr_port_regs(comm->port_id);
	gpi_status_reg = (void *)&regs[GPI_SMI_STS_OFFSET];
	gpi_en_reg = (void *)&regs[GPI_SMI_EN_OFFSET];

	/* Offset of SMI STS/EN for this pad's group within the community. */
	group_offset = (pad - comm->min) / GPIO_MAX_NUM_PER_GROUP;

	/* Clear status then set enable. */
	pad_mask = 1 << ((pad - comm->min) % GPIO_MAX_NUM_PER_GROUP);
	write32(&gpi_status_reg[group_offset], pad_mask);
	write32(&gpi_en_reg[group_offset],
		read32(&gpi_en_reg[group_offset]) | pad_mask);
}

static void gpio_configure_pad(const struct pad_config *cfg)
{
	uint32_t *dw_regs;
	uint32_t reg;
	uint32_t termination;
	uint32_t dw0;
	const uint32_t termination_mask = PAD_TERM_MASK << PAD_TERM_SHIFT;

	dw_regs = gpio_dw_regs(cfg->pad);

	if (dw_regs == NULL)
		return;

	dw0 = cfg->dw0;

	write32(&dw_regs[0], dw0);
	reg = read32(&dw_regs[1]);
	reg &= ~termination_mask;
	termination = cfg->attrs;
	termination &= termination_mask;
	reg |= termination;
	write32(&dw_regs[1], reg);

	gpio_handle_pad_mode(cfg);

	if ((dw0 & PAD_FIELD(GPIROUTSMI, MASK)) == PAD_FIELD(GPIROUTSMI, YES))
		gpi_enable_smi(cfg->pad);

	if(gpio_debug)
		printk(BIOS_DEBUG,
			"Write Pad: Base(%p) - conf0 = %x conf1= %x pad # = %d\n",
			&dw_regs[0], dw0, reg, cfg->pad);
}

void gpio_configure_pads(const struct pad_config *cfgs, size_t num)
{
	size_t i;

	for (i = 0; i < num; i++)
		gpio_configure_pad(&cfgs[i]);
}

void gpio_input_pulldown(gpio_t gpio)
{
	struct pad_config cfg = PAD_CFG_GPI(gpio, 5K_PD, DEEP);
	gpio_configure_pad(&cfg);
}

void gpio_input_pullup(gpio_t gpio)
{
	struct pad_config cfg = PAD_CFG_GPI(gpio, 5K_PU, DEEP);
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
	uint32_t *dw_regs;
	uint32_t reg;

	dw_regs = gpio_dw_regs(gpio_num);

	if (dw_regs == NULL)
		return -1;

	reg = read32(&dw_regs[0]);

	return (reg >> GPIORXSTATE_SHIFT) & GPIORXSTATE_MASK;
}

void gpio_set(gpio_t gpio_num, int value)
{
	uint32_t *dw_regs;
	uint32_t reg;

	dw_regs = gpio_dw_regs(gpio_num);

	if (dw_regs == NULL)
		return;

	reg = read32(&dw_regs[0]);
	reg &= ~PAD_FIELD(GPIOTXSTATE, MASK);
	reg |= PAD_FIELD_VAL(GPIOTXSTATE, value);
	write32(&dw_regs[0], reg);
	/* GPIO port ids support posted write semantics. */
}
