/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8188 Functional Specification
 * Chapter number: 5.2
 */

#include <device/mmio.h>
#include <gpio.h>

void *gpio_find_reg_addr(gpio_t gpio)
{
	void *reg_addr;
	switch (gpio.base & 0x0f) {
	case 1:
		reg_addr = (void *)IOCFG_RM_BASE;
		break;
	case 2:
		reg_addr = (void *)IOCFG_LT_BASE;
		break;
	case 3:
		reg_addr = (void *)IOCFG_LM_BASE;
		break;
	case 4:
		reg_addr = (void *)IOCFG_RT_BASE;
		break;
	default:
		reg_addr = NULL;
		break;
	}

	return reg_addr;
}
