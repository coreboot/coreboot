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

/* Keep the ordering intact GPP_A ~ G, GPD.
 * As the gpio/smi functions get_smi_status() and
 * enable_gpio_groupsmi() depends on this ordering.
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
	/* GPP_H */
	{
		.community = PID_GPIOCOM2,
		.padcfgoffset = R_PCH_PCR_GPIO_GPD_PADCFG_OFFSET,
		.padpergroup = V_PCH_GPIO_GPD_PAD_MAX,
		.smistsoffset = NO_REGISTER_PROPERTY,
		.smienoffset = NO_REGISTER_PROPERTY,
	},
};

/*
 * SPT has 7 GPIO communities named as GPP_A to GPP_G.
 * Each community has 24 GPIO PIN.
 * Below formula to calculate GPIO Pin from GPIO PAD.
 * PIN# = GROUP_PAD# + GROUP# * 24
 * ====================================
 * Community || Group#
 * ====================================
 * GPP_A	||	0
 * GPP_B	||	1
 * GPP_C	||	2
 * GPP_D	||	3
 * GPP_E	||	4
 * GPP_F	||	5
 * GPP_G	||	6
 */
static u32 get_padnumber_from_gpiopad(GPIO_PAD gpiopad)
{
	return (u32) GPIO_GET_PAD_NUMBER(gpiopad);
}

static u32 get_groupindex_from_gpiopad(GPIO_PAD gpiopad)
{
	return (u32) GPIO_GET_GROUP_INDEX_FROM_PAD(gpiopad);
}

static int read_write_gpio_pad_reg(u32 gpiopad, u8 dwreg, u32 mask, int write,
			    u32 *readwriteval)
{
	u32 padcfgreg;
	u32 gpiogroupinfolength;
	u32 groupindex;
	u32 padnumber;

	groupindex = get_groupindex_from_gpiopad(gpiopad);
	padnumber = get_padnumber_from_gpiopad(gpiopad);

	gpiogroupinfolength = sizeof(gpio_group_info) / sizeof(GPIO_GROUP_INFO);

	/* Check if group argument exceeds GPIO GROUP INFO array */
	if ((u32) groupindex >= gpiogroupinfolength)
		return -1;
	/* Check if legal pin number */
	if (padnumber >= gpio_group_info[groupindex].padpergroup)
		return -1;
	/* Create Pad Configuration register offset */
	padcfgreg = 0x8 * padnumber + gpio_group_info[groupindex].padcfgoffset;
	if (dwreg == 1)
		padcfgreg += 0x4;
	if (write) {
		pcr_andthenor32(gpio_group_info[groupindex].community,
				padcfgreg, (u32) (~mask),
				(u32) (*readwriteval & mask));
	} else {
		pcr_read32(gpio_group_info[groupindex].community, padcfgreg,
			   readwriteval);
		*readwriteval &= mask;
	}

	return 0;
}

static int convert_gpio_num_to_pad(gpio_t gpionum)
{
	int group_pad_num = 0;
	int gpio_group = 0;
	u32 gpio_pad = 0;

	group_pad_num = (gpionum % MAX_GPIO_PIN_PER_GROUP);
	gpio_group = (gpionum / MAX_GPIO_PIN_PER_GROUP);

	switch (gpio_group) {
	case GPIO_LP_GROUP_A:
		gpio_pad = GPIO_LP_GROUP_GPP_A;
		break;

	case GPIO_LP_GROUP_B:
		gpio_pad = GPIO_LP_GROUP_GPP_B;
		break;

	case GPIO_LP_GROUP_C:
		gpio_pad = GPIO_LP_GROUP_GPP_C;
		break;

	case GPIO_LP_GROUP_D:
		gpio_pad = GPIO_LP_GROUP_GPP_D;
		break;

	case GPIO_LP_GROUP_E:
		gpio_pad = GPIO_LP_GROUP_GPP_E;
		break;

	case GPIO_LP_GROUP_F:
		gpio_pad = GPIO_LP_GROUP_GPP_F;
		break;

	case GPIO_LP_GROUP_G:
		gpio_pad = GPIO_LP_GROUP_GPP_G;
		break;
	default:
		return -1;
		break;
	}
	gpio_pad = (gpio_pad << GPIO_GROUP_SHIFT) + group_pad_num;

	return gpio_pad;
}

int gpio_get(gpio_t gpio_num)
{
	u32 gpiopad = 0;
	u32 outputvalue = 0;
	int status = 0;

	if (gpio_num > MAX_GPIO_NUMBER)
		return 0;

	gpiopad = convert_gpio_num_to_pad(gpio_num);
	if (gpiopad < 0)
		return -1;

	status = read_write_gpio_pad_reg(gpiopad,
					 0,
					 B_PCH_GPIO_TX_STATE,
					 READ, &outputvalue);
	outputvalue >>= N_PCH_GPIO_TX_STATE;
	return outputvalue;
}

void gpio_set(gpio_t gpio_num, int value)
{
	int status = 0;
	u32 gpiopad = 0;
	u32 outputvalue = 0;

	if (gpio_num > MAX_GPIO_NUMBER)
		return;

	gpiopad = convert_gpio_num_to_pad(gpio_num);
	if (gpiopad < 0)
		return;

	outputvalue = value;

	status = read_write_gpio_pad_reg(gpiopad,
					 0,
					 B_PCH_GPIO_TX_STATE,
					 WRITE, &outputvalue);
}

void clear_all_smi(void)
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

void get_smi_status(u32 status[GPIO_COMMUNITY_MAX])
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

void enable_all_smi(void)
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

void enable_gpio_groupsmi(gpio_t gpio_num, u32 mask)
{
	u32 gpioindex = 0;
	u32 smien = 0;

	if (gpio_num > MAX_GPIO_NUMBER)
		return;

	gpioindex = (gpio_num / MAX_GPIO_PIN_PER_GROUP);

	pcr_read32(gpio_group_info[gpioindex].community,
		   gpio_group_info[gpioindex].smienoffset, &smien);
	smien |= mask;
	/* Set all GPI SMI Enable bits by writing '1' */
	pcr_write32(gpio_group_info[gpioindex].community,
		    gpio_group_info[gpioindex].smienoffset, smien);
}
