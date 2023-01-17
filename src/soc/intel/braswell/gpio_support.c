/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <console/console.h>
#include <gpio.h>

/*
 * Return family number and internal pad number in that community by pad number
 * and which community it is in.
 */
uint16_t gpio_family_number(uint8_t community, uint8_t pad)
{
	/*
	 * Refer to BSW BIOS Writers Guide, Table "Family Number". BSW has 4 GPIO communities.
	 * Each community has up to 7 families and each family contains a range of Pad numbers.
	 * The number in the array is the maximum no. of that range.
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
 * Return pad configuration register offset by pad number and which community it is in.
 */
uint32_t *gpio_pad_config_reg(uint8_t community, uint8_t pad)
{
	uint16_t fpad;
	uint32_t *pad_config_reg;

	/* Get the GPIO family number */
	fpad = gpio_family_number(community, pad);

	/*
	 * Refer to BSW BIOS Writers Guide, Table "Per Pad Memory Space Registers Addresses"
	 * for the Pad configuration register calculation.
	 */
	pad_config_reg = (uint32_t *)(COMMUNITY_BASE(community) + FAMILY_PAD_REGS_OFF +
		(FAMILY_PAD_REGS_SIZE * (fpad >> 8)) + (GPIO_REGS_SIZE * (fpad & 0xff)));

	return pad_config_reg;
}

static int gpio_get_community_num(gpio_t gpio_num, int *pad)
{
	int comm = 0;

	if (gpio_num >= GP_SW_00 && gpio_num <= GP_SW_97) {
		comm =  GP_SOUTHWEST;
		*pad = gpio_num % GP_SOUTHWEST_COUNT;

	} else if (gpio_num >= GP_NC_00 && gpio_num <= GP_NC_72) {
		comm =  GP_NORTH;
		*pad = gpio_num % GP_SOUTHWEST_COUNT;

	} else if (gpio_num >= GP_E_00 && gpio_num <= GP_E_26) {
		comm =  GP_EAST;
		*pad = gpio_num % (GP_SOUTHWEST_COUNT + GP_NORTH_COUNT);

	} else {
		comm = GP_SOUTHEAST;
		*pad = gpio_num % (GP_SOUTHWEST_COUNT + GP_NORTH_COUNT + GP_EAST_COUNT);
	}
	return comm;
}

static void gpio_config_pad(gpio_t gpio_num, const struct soc_gpio_map *cfg)
{
	int comm = 0;
	int pad_num = 0;
	uint32_t *pad_config0_reg;
	uint32_t *pad_config1_reg;

	if (gpio_num > MAX_GPIO_CNT)
		return;
	/* Get GPIO Community based on GPIO_NUMBER */
	comm = gpio_get_community_num(gpio_num, &pad_num);
	/* CONF0 */
	pad_config0_reg = gpio_pad_config_reg(comm, pad_num);
	/* CONF1 */
	pad_config1_reg = pad_config0_reg + 1;

	write32(pad_config0_reg, cfg->pad_conf0);
	write32(pad_config1_reg, cfg->pad_conf1);
}

void gpio_input_pullup(gpio_t gpio_num)
{
	struct soc_gpio_map cfg = GPIO_INPUT_PU_20K;
	gpio_config_pad(gpio_num, &cfg);
}

void gpio_input_pulldown(gpio_t gpio_num)
{
	struct soc_gpio_map cfg = GPIO_INPUT_PD_20K;
	gpio_config_pad(gpio_num, &cfg);
}

void gpio_input(gpio_t gpio_num)
{
	struct soc_gpio_map cfg = GPIO_INPUT_NO_PULL;
	gpio_config_pad(gpio_num, &cfg);
}

int gpio_get(gpio_t gpio_num)
{
	int comm = 0;
	int pad_num = 0;
	uint32_t *pad_config0_reg;
	u32 pad_value;

	if (gpio_num > MAX_GPIO_CNT)
		return -1;

	/* Get GPIO Community based on GPIO_NUMBER */
	comm = gpio_get_community_num(gpio_num, &pad_num);
	/* CONF0 */
	pad_config0_reg = gpio_pad_config_reg(comm, pad_num);

	pad_value = read32(pad_config0_reg);

	return pad_value & PAD_RX_BIT;
}

int get_gpio(int community_base, int pad0_offset)
{
	return (read32((void *)(community_base + pad0_offset))) & PAD_RX_BIT;
}
