/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <gpio.h>

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
			setbits32(reg1, BIT(bit));
		else
			clrbits32(reg1, BIT(bit));
	}

	if (enable == GPIO_PULL_ENABLE) {
		setbits32(reg2, BIT(bit));
	} else {
		clrbits32(reg2, BIT(bit));
		clrbits32(reg2 + 0x010, BIT(bit));
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
			clrbits32(reg1, BIT(bit));
			setbits32(reg2, BIT(bit));
		} else {
			clrbits32(reg2, BIT(bit));
			setbits32(reg1, BIT(bit));
		}
	} else {
		clrbits32(reg1, BIT(bit));
		clrbits32(reg2, BIT(bit));
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
