/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <gpio.h>
#include <assert.h>

static void *gpio_find_reg_addr(gpio_t gpio)
{
	void *reg_addr;
	switch (gpio.base & 0x0f) {
	case 0:
		reg_addr = (void *)IOCFG_RM_BASE;
		break;
	case 1:
		reg_addr = (void *)IOCFG_BM_BASE;
		break;
	case 2:
		reg_addr = (void *)IOCFG_BL_BASE;
		break;
	case 3:
		reg_addr = (void *)IOCFG_BR_BASE;
		break;
	case 4:
		reg_addr = (void *)IOCFG_LM_BASE;
		break;
	case 5:
		reg_addr = (void *)IOCFG_LB_BASE;
		break;
	case 6:
		reg_addr = (void *)IOCFG_RT_BASE;
		break;
	case 7:
		reg_addr = (void *)IOCFG_LT_BASE;
		break;
	case 8:
		reg_addr = (void *)IOCFG_TL_BASE;
		break;
	default:
		reg_addr = NULL;
		break;
	}

	return reg_addr;
}

static void gpio_set_spec_pull_pupd(gpio_t gpio, enum pull_enable enable,
				    enum pull_select select)
{
	void *reg1;
	void *reg2;
	int bit = gpio.bit;

	reg1 = gpio_find_reg_addr(gpio) + gpio.offset;
	reg2 = reg1 + (gpio.base & 0xf0);

	if (enable == GPIO_PULL_ENABLE) {
		if (select == GPIO_PULL_DOWN)
			setbits32(reg1, 1 << bit);
		else
			clrbits32(reg1, 1 << bit);
	}

	if (enable == GPIO_PULL_ENABLE)
		setbits32(reg2, 1 << bit);
	else {
		clrbits32(reg2, 1 << bit);
		clrbits32(reg2 + 0x010, 1 << bit);
	}
}

static void gpio_set_pull_pu_pd(gpio_t gpio, enum pull_enable enable,
				enum pull_select select)
{
	void *reg1;
	void *reg2;
	int bit = gpio.bit;

	reg1 = gpio_find_reg_addr(gpio) + gpio.offset;
	reg2 = reg1 - (gpio.base & 0xf0);

	if (enable == GPIO_PULL_ENABLE) {
		if (select == GPIO_PULL_DOWN) {
			clrbits32(reg1, 1 << bit);
			setbits32(reg2, 1 << bit);
		} else {
			clrbits32(reg2, 1 << bit);
			setbits32(reg1, 1 << bit);
		}
	} else {
		clrbits32(reg1, 1 << bit);
		clrbits32(reg2, 1 << bit);
	}
}

void gpio_set_pull(gpio_t gpio, enum pull_enable enable,
		   enum pull_select select)
{
	if (gpio.flag)
		gpio_set_spec_pull_pupd(gpio, enable, select);
	else
		gpio_set_pull_pu_pd(gpio, enable, select);
}
