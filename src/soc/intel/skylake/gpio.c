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
#include <device/device.h>
#include <device/pci.h>
#include <gpio.h>
#include <soc/pcr.h>
#include <soc/iomap.h>
#include <soc/pm.h>


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
	reg |= PAD_FIELD_VAL(GPIOTXSTATE, value);
	write32(&dw_regs[0], reg);
	/* GPIO port ids support posted write semantics. */
}

/* Keep the ordering intact GPP_A ~ G, GPD.
 * As the gpio/smi functions gpio_get_smi_status() and
 * gpio_enable_groupsmi() depends on this ordering.
 */
static const GPIO_GROUP_INFO gpio_group_info[] = {
	/* GPP_A */
	{
		.community = PID_GPIOCOM0,
		.padcfgoffset = R_PCH_PCR_GPIO_GPP_A_PADCFG_OFFSET,
		.padpergroup = V_PCH_GPIO_GPP_A_PAD_MAX,
		.smistsoffset = R_PCH_PCR_GPIO_GPP_A_SMI_STS,
		.smienoffset = R_PCH_PCR_GPIO_GPP_A_SMI_EN,
	},
	/* GPP_B */
	{
		.community = PID_GPIOCOM0,
		.padcfgoffset = R_PCH_PCR_GPIO_GPP_B_PADCFG_OFFSET,
		.padpergroup = V_PCH_GPIO_GPP_B_PAD_MAX,
		.smistsoffset = R_PCH_PCR_GPIO_GPP_B_SMI_STS,
		.smienoffset = R_PCH_PCR_GPIO_GPP_B_SMI_EN,
	},
	/* GPP_C */
	{
		.community = PID_GPIOCOM1,
		.padcfgoffset = R_PCH_PCR_GPIO_GPP_C_PADCFG_OFFSET,
		.padpergroup = V_PCH_GPIO_GPP_C_PAD_MAX,
		.smistsoffset = R_PCH_PCR_GPIO_GPP_C_SMI_STS,
		.smienoffset = R_PCH_PCR_GPIO_GPP_C_SMI_EN,
	},
	/* GPP_D */
	{
		.community = PID_GPIOCOM1,
		.padcfgoffset = R_PCH_PCR_GPIO_GPP_D_PADCFG_OFFSET,
		.padpergroup = V_PCH_GPIO_GPP_D_PAD_MAX,
		.smistsoffset = R_PCH_PCR_GPIO_GPP_D_SMI_STS,
		.smienoffset = R_PCH_PCR_GPIO_GPP_D_SMI_EN,
	},
	/* GPP_E */
	{
		.community = PID_GPIOCOM1,
		.padcfgoffset = R_PCH_PCR_GPIO_GPP_E_PADCFG_OFFSET,
		.padpergroup = V_PCH_GPIO_GPP_E_PAD_MAX,
		.smistsoffset = R_PCH_PCR_GPIO_GPP_E_SMI_STS,
		.smienoffset = R_PCH_PCR_GPIO_GPP_E_SMI_EN,
	},
	/* GPP_F */
	{
		.community = PID_GPIOCOM3,
		.padcfgoffset = R_PCH_PCR_GPIO_GPP_F_PADCFG_OFFSET,
		.padpergroup = V_PCH_GPIO_GPP_F_PAD_MAX,
		.smistsoffset = NO_REGISTER_PROPERTY,
		.smienoffset = NO_REGISTER_PROPERTY,
	},
	/* GPP_G */
	{
		.community = PID_GPIOCOM3,
		.padcfgoffset = R_PCH_PCR_GPIO_GPP_G_PADCFG_OFFSET,
		.padpergroup = V_PCH_GPIO_GPP_G_PAD_MAX,
		.smistsoffset = NO_REGISTER_PROPERTY,
		.smienoffset = NO_REGISTER_PROPERTY,
	},
	/* GPD */
	{
		.community = PID_GPIOCOM2,
		.padcfgoffset = R_PCH_PCR_GPIO_GPD_PADCFG_OFFSET,
		.padpergroup = V_PCH_GPIO_GPD_PAD_MAX,
		.smistsoffset = NO_REGISTER_PROPERTY,
		.smienoffset = NO_REGISTER_PROPERTY,
	},
};

void gpio_clear_all_smi(void)
{
	u32 gpiogroupinfolength;
	u32 gpioindex = 0;

	gpiogroupinfolength = sizeof(gpio_group_info) / sizeof(GPIO_GROUP_INFO);

	for (gpioindex = 0; gpioindex < gpiogroupinfolength; gpioindex++) {
		/*Check if group has GPI SMI register */
		if (gpio_group_info[gpioindex].smistsoffset ==
		    NO_REGISTER_PROPERTY)
			continue;
		/* Clear all GPI SMI Status bits by writing '1' */
		pcr_write32(gpio_group_info[gpioindex].community,
			    gpio_group_info[gpioindex].smistsoffset,
			    0xFFFFFFFF);
	}
}

void gpio_get_smi_status(u32 status[GPIO_COMMUNITY_MAX])
{
	u32 num_of_communities;
	u32 gpioindex;
	u32 outputvalue = 0;

	num_of_communities = ARRAY_SIZE(gpio_group_info);

	for (gpioindex = 0; gpioindex < num_of_communities; gpioindex++) {
		/*Check if group has GPI SMI register */
		if (gpio_group_info[gpioindex].smistsoffset ==
		    NO_REGISTER_PROPERTY)
			continue;
		/* Read SMI status register */
		pcr_read32(gpio_group_info[gpioindex].community,
			   gpio_group_info[gpioindex].smistsoffset,
			   &outputvalue);
		status[gpioindex] = outputvalue;
	}
}

void gpio_enable_all_smi(void)
{
	u32 gpiogroupinfolength;
	u32 gpioindex = 0;

	gpiogroupinfolength = sizeof(gpio_group_info) / sizeof(GPIO_GROUP_INFO);

	for (gpioindex = 0; gpioindex < gpiogroupinfolength; gpioindex++) {
		/*Check if group has GPI SMI register */
		if (gpio_group_info[gpioindex].smienoffset ==
		    NO_REGISTER_PROPERTY)
			continue;
		/* Set all GPI SMI Enable bits by writing '1' */
		pcr_write32(gpio_group_info[gpioindex].community,
			    gpio_group_info[gpioindex].smienoffset,
			    0xFFFFFFFF);
	}
}
