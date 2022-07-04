/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 5.1
 */

#include <device/mmio.h>
#include <gpio.h>

static const struct gpio_drv_info gpio_driving_info[] = {
	[0] = { 0x0, 27, 3, },
	[1] = { 0x0, 27, 3, },
	[2] = { 0x0, 27, 3, },
	[3] = { 0x0, 27, 3, },
	[4] = { 0x10, 0, 3, },
	[5] = { 0x10, 0, 3, },
	[6] = { 0x0, 9, 3, },
	[7] = { 0x0, 9, 3, },
	[8] = { 0x0, 9, 3, },
	[9] = { 0x0, 12, 3, },
	[10] = { 0x0, 0, 3, },
	[11] = { 0x0, 3, 3, },
	[12] = { 0x0, 6, 3, },
	[13] = { 0x0, 0, 3, },
	[14] = { 0x0, 3, 3, },
	[15] = { 0x10, 3, 3, },
	[16] = { 0x10, 3, 3, },
	[17] = { 0x0, 21, 3, },
	[18] = { 0x10, 0, 3, },
	[19] = { 0x0, 27, 3, },
	[20] = { 0x0, 24, 3, },
	[21] = { 0x0, 27, 3, },
	[22] = { 0x0, 24, 3, },
	[23] = { 0x10, 0, 3, },
	[24] = { 0x10, 9, 3, },
	[25] = { 0x10, 0, 3, },
	[26] = { 0x0, 27, 3, },
	[27] = { 0x0, 27, 3, },
	[28] = { 0x10, 0, 3, },
	[29] = { 0x10, 0, 3, },
	[30] = { 0x10, 0, 3, },
	[31] = { 0x10, 3, 3, },
	[32] = { 0x0, 6, 3, },
	[33] = { 0x0, 6, 3, },
	[34] = { 0x0, 3, 3, },
	[35] = { 0x0, 3, 3, },
	[36] = { 0x0, 9, 3, },
	[37] = { 0x0, 9, 3, },
	[38] = { 0x0, 12, 3, },
	[39] = { 0x0, 9, 3, },
	[40] = { 0x0, 15, 3, },
	[41] = { 0x0, 12, 3, },
	[42] = { 0x0, 12, 3, },
	[43] = { 0x0, 12, 3, },
	[44] = { 0x10, 6, 3, },
	[45] = { 0x10, 3, 3, },
	[46] = { 0x10, 3, 3, },
	[47] = { 0x10, 6, 3, },
	[48] = { 0x0, 15, 3, },
	[49] = { 0x0, 15, 3, },
	[50] = { 0x0, 15, 3, },
	[51] = { 0x0, 18, 3, },
	[52] = { 0x10, 3, 3, },
	[53] = { 0x10, 6, 3, },
	[54] = { 0x10, 3, 3, },
	[55] = { 0x10, 3, 3, },
	[56] = { 0x10, 3, 3, },
	[57] = { 0x10, 6, 3, },
	[58] = { 0x10, 6, 3, },
	[59] = { 0x10, 3, 3, },
	[60] = { 0x0, 24, 3, },
	[61] = { 0x0, 24, 3, },
	[62] = { 0x0, 24, 3, },
	[63] = { 0x0, 24, 3, },
	[64] = { 0x0, 27, 3, },
	[65] = { 0x0, 27, 3, },
	[66] = { 0x0, 27, 3, },
	[67] = { 0x10, 0, 3, },
	[68] = { 0x0, 0, 3, },
	[69] = { 0x0, 3, 3, },
	[70] = { 0x10, 3, 3, },
	[71] = { 0x0, 6, 3, },
	[72] = { 0x0, 9, 3, },
	[73] = { 0x0, 12, 3, },
	[74] = { 0x0, 15, 3, },
	[75] = { 0x0, 18, 3, },
	[76] = { 0x0, 21, 3, },
	[77] = { 0x0, 24, 3, },
	[78] = { 0x0, 27, 3, },
	[79] = { 0x0, 24, 3, },
	[80] = { 0x0, 24, 3, },
	[81] = { 0x0, 21, 3, },
	[82] = { 0x0, 21, 3, },
	[83] = { 0x10, 3, 3, },
	[84] = { 0x0, 6, 3, },
	[85] = { 0x0, 9, 3, },
	[86] = { 0x0, 12, 3, },
	[87] = { 0x0, 15, 3, },
	[88] = { 0x0, 18, 3, },
	[89] = { 0x0, 21, 3, },
	[90] = { 0x0, 27, 3, },
	[91] = { 0x10, 0, 3, },
	[92] = { 0x10, 0, 3, },
	[93] = { 0x10, 0, 3, },
	[94] = { 0x10, 0, 3, },
	[95] = { 0x10, 3, 3, },
	[96] = { 0x0, 9, 3, },
	[97] = { 0x0, 6, 3, },
	[98] = { 0x0, 3, 3, },
	[99] = { 0x0, 18, 3, },
	[100] = { 0x0, 18, 3, },
	[101] = { 0x0, 21, 3, },
	[102] = { 0x0, 21, 3, },
	[103] = { 0x0, 21, 3, },
	[104] = { 0x0, 21, 3, },
	[105] = { 0x0, 24, 3, },
	[106] = { 0x0, 24, 3, },
	[107] = { 0x0, 24, 3, },
	[108] = { 0x0, 24, 3, },
	[109] = { 0x0, 27, 3, },
	[110] = { 0x0, 27, 3, },
	[111] = { 0x0, 27, 3, },
	[112] = { 0x0, 27, 3, },
	[113] = { 0x10, 0, 3, },
	[114] = { 0x10, 0, 3, },
	[115] = { 0x10, 6, 3, },
	[116] = { 0x10, 9, 3, },
	[117] = { 0x10, 6, 3, },
	[118] = { 0x10, 6, 3, },
	[119] = { 0x0, 18, 3, },
	[120] = { 0x0, 9, 3, },
	[121] = { 0x0, 12, 3, },
	[122] = { 0x0, 15, 3, },
	[123] = { 0x0, 21, 3, },
	[124] = { 0x0, 0, 3, },
	[125] = { 0x0, 3, 3, },
	[126] = { 0x0, 6, 3, },
	[127] = { 0x10, 12, 3, },
	[128] = { 0x10, 18, 3, },
	[129] = { 0x10, 12, 3, },
	[130] = { 0x10, 18, 3, },
	[131] = { 0x10, 15, 3, },
	[132] = { 0x10, 21, 3, },
	[133] = { 0x10, 15, 3, },
	[134] = { 0x10, 21, 3, },
	[135] = { 0x10, 6, 3, },
	[136] = { 0x10, 15, 3, },
	[137] = { 0x10, 18, 3, },
	[138] = { 0x10, 24, 3, },
	[139] = { 0x0, 21, 3, },
	[140] = { 0x0, 24, 3, },
	[141] = { 0x10, 15, 3, },
	[142] = { 0x10, 21, 3, },
	[143] = { 0x10, 9, 3, },
	[144] = { 0x10, 18, 3, },
	[145] = { 0x10, 12, 3, },
	[146] = { 0x10, 21, 3, },
	[147] = { 0x0, 12, 3, },
	[148] = { 0x0, 12, 3, },
	[149] = { 0x0, 12, 3, },
	[150] = { 0x0, 15, 3, },
	[151] = { 0x0, 15, 3, },
	[152] = { 0x0, 9, 3, },
	[153] = { 0x0, 15, 3, },
	[154] = { 0x0, 15, 3, },
	[155] = { 0x0, 18, 3, },
	[156] = { 0x0, 18, 3, },
	[157] = { 0x0, 0, 3, },
	[158] = { 0x0, 0, 3, },
	[159] = { 0x0, 0, 3, },
	[160] = { 0x0, 0, 3, },
	[161] = { 0x0, 6, 3, },
	[162] = { 0x0, 3, 3, },
	[163] = { 0x10, 12, 3, },
	/* 164 is unimplemented */
	[165] = { 0x10, 6, 3, },
	[166] = { 0x10, 6, 3, },
	[167] = { 0x10, 9, 3, },
	[168] = { 0x10, 6, 3, },
	[169] = { 0x10, 12, 3, },
	[170] = { 0x10, 9, 3, },
	[171] = { 0x10, 9, 3, },
	[172] = { 0x10, 9, 3, },
	[173] = { 0x10, 6, 3, },
	[174] = { 0x0, 9, 3, },
	[175] = { 0x0, 12, 3, },
	[176] = { 0x0, 0, 3, },
	[177] = { 0x0, 3, 3, },
	[178] = { 0x0, 6, 3, },
	[179] = { 0x0, 15, 3, },
	[180] = { 0x0, 18, 3, },
	[181] = { 0x0, 21, 3, },
	[182] = { 0x0, 24, 3, },
	[183] = { 0x0, 27, 3, },
	[184] = { 0x10, 0, 3, },
};

_Static_assert(ARRAY_SIZE(gpio_driving_info) == GPIO_NUM,
	       "gpio_driving_info array size not match");

/* Unimplemented GPIOs are intentionally omitted here with width=0 */
static const struct gpio_drv_info gpio_driving_adv_info[GPIO_NUM] = {
	[127] = { 0x30, 0, 3, },
	[128] = { 0x30, 6, 3, },
	[129] = { 0x30, 0, 3, },
	[130] = { 0x30, 6, 3, },
	[131] = { 0x30, 3, 3, },
	[132] = { 0x30, 9, 3, },
	[133] = { 0x30, 0, 3, },
	[134] = { 0x30, 6, 3, },
	[135] = { 0x20, 0, 3, },
	[136] = { 0x20, 9, 3, },
	[137] = { 0x30, 3, 3, },
	[138] = { 0x30, 9, 3, },
	[139] = { 0x20, 0, 3, },
	[140] = { 0x20, 3, 3, },
	[141] = { 0x30, 3, 3, },
	[142] = { 0x30, 9, 3, },
	[143] = { 0x20, 3, 3, },
	[144] = { 0x20, 12, 3, },
	[145] = { 0x20, 6, 3, },
	[146] = { 0x20, 15, 3, },
};

void *gpio_find_reg_addr(gpio_t gpio)
{
	void *reg_addr;
	switch (gpio.base & 0x0f) {
	case 1:
		reg_addr = (void *)IOCFG_LT_BASE;
		break;
	case 2:
		reg_addr = (void *)IOCFG_LM_BASE;
		break;
	case 3:
		reg_addr = (void *)IOCFG_LB_BASE;
		break;
	case 4:
		reg_addr = (void *)IOCFG_BL_BASE;
		break;
	case 5:
		reg_addr = (void *)IOCFG_RB_BASE;
		break;
	case 6:
		reg_addr = (void *)IOCFG_RT_BASE;
		break;
	default:
		reg_addr = NULL;
		break;
	}

	return reg_addr;
}

static inline bool is_valid_drv(uint8_t drv)
{
	return drv <= GPIO_DRV_16_MA;
}

static inline bool is_valid_drv_adv(enum gpio_drv_adv drv)
{
	return drv <= GPIO_DRV_ADV_1_MA && drv >= GPIO_DRV_ADV_125_UA;
}

int gpio_set_driving(gpio_t gpio, uint8_t drv)
{
	uint32_t mask;
	const struct gpio_drv_info *info = &gpio_driving_info[gpio.id];
	const struct gpio_drv_info *adv_info = &gpio_driving_adv_info[gpio.id];
	void *reg, *reg_adv;

	if (!is_valid_drv(drv))
		return -1;

	reg = gpio_find_reg_addr(gpio) + info->offset;
	reg_adv = gpio_find_reg_addr(gpio) + adv_info->offset;

	if (info->width == 0)
		return -1;

	mask = BIT(info->width) - 1;
	/* Check setting value is not beyond width */
	if ((uint32_t)drv > mask)
		return -1;

	clrsetbits32(reg, mask << info->shift, drv << info->shift);

	/* Disable EH if supported */
	if (adv_info->width != 0)
		clrbits32(reg_adv, BIT(adv_info->shift));

	return 0;
}

int gpio_get_driving(gpio_t gpio)
{
	const struct gpio_drv_info *info = &gpio_driving_info[gpio.id];
	void *reg;

	reg = gpio_find_reg_addr(gpio) + info->offset;
	if (info->width == 0)
		return -1;

	return (read32(reg) >> info->shift) & (BIT(info->width) - 1);
}

int gpio_set_driving_adv(gpio_t gpio, enum gpio_drv_adv drv)
{
	uint32_t mask;
	const struct gpio_drv_info *adv_info = &gpio_driving_adv_info[gpio.id];
	void *reg_adv;

	if (!is_valid_drv_adv(drv))
		return -1;

	reg_adv = gpio_find_reg_addr(gpio) + adv_info->offset;

	if (adv_info->width == 0)
		return -1;

	/* Not include EH bit (the lowest bit) */
	if ((uint32_t)drv > (BIT(adv_info->width - 1) - 1))
		return -1;

	mask = BIT(adv_info->width) - 1;
	/* EH enable */
	drv = (drv << 1) | BIT(0);

	clrsetbits32(reg_adv, mask << adv_info->shift, drv << adv_info->shift);

	return 0;
}

int gpio_get_driving_adv(gpio_t gpio)
{
	const struct gpio_drv_info *adv_info = &gpio_driving_adv_info[gpio.id];
	void *reg_adv;
	uint32_t drv;

	reg_adv = gpio_find_reg_addr(gpio) + adv_info->offset;
	if (adv_info->width == 0)
		return -1;

	drv = (read32(reg_adv) >> adv_info->shift) & (BIT(adv_info->width) - 1);

	/* Drop EH bit */
	return drv >> 1;
}
