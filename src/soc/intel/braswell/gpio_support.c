/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <console/console.h>
#include <soc/gpio.h>

/*
 * Return family number and internal pad number in that community by pad number
 * and which community it is in.
 */
uint16_t gpio_family_number(uint8_t community, uint8_t pad)
{
	/*
	 * Refer to BSW BIOS Writers Guide, Table "Family Number".
	 * BSW has 4 GPIO communities. Each community has up to 7 families and
	 * each family contains a range of Pad numbers. The number in the array
	 * is the maximum no. of that range.
	 * For example: East community, family 0, Pad 0~11.
	 */
	static const uint8_t community_base[GPIO_COMMUNITY_COUNT]
		[GPIO_FAMILIES_MAX_PER_COMM + 1] = {
		{0,  8, 16, 24, 32, 40, 48, 56}, /* Southwest */
		{0,  9, 22, 34, 46, 59, 59, 59}, /* North */
		{0, 12, 24, 24, 24, 24, 24, 24}, /* East */
		{0,  8, 20, 26, 34, 44, 55, 55}  /* Southeast */
	};
	const uint8_t *base;
	uint8_t i;

	/* Validate the pad number */
	if (pad > community_base[community][7])
		die("Pad number is out of range!");

	/* Locate the family number for the pad */
	base = &community_base[community][0];
	for (i = 0; i < 7; i++) {
		if ((pad >= base[0]) && (pad < base[1]))
			break;
		base++;
	}

	/* Family number in high byte and inner pad number in lowest byte */
	return (i << 8) + pad - *base;
}

/*
 * Return pad configuration register offset by pad number and which community
 * it is in.
 */
uint32_t *gpio_pad_config_reg(uint8_t community, uint8_t pad)
{
	uint16_t fpad;
	uint32_t *pad_config_reg;

	/* Get the GPIO family number */
	fpad = gpio_family_number(community, pad);

	/*
	 * Refer to BSW BIOS Writers Guide, Table "Per Pad Memory Space
	 * Registers Addresses" for the Pad configuration register calculation.
	 */
	pad_config_reg = (uint32_t *)(COMMUNITY_BASE(community) + 0x4400 +
		(0x400 * (fpad >> 8)) + (8 * (fpad & 0xff)));

	return pad_config_reg;
}

int get_gpio(int community_base, int pad0_offset)
{
	return (read32((void *)(community_base + pad0_offset))) & PAD_RX_BIT;
}
