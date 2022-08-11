/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/mmio.h>
#include <gpio.h>

void *gpio_find_reg_addr(gpio_t gpio)
{
	void *reg_addr;
	switch (gpio.base & 0x0f) {
	case 0:
		reg_addr = (void *)IOCFG_BL_BASE;
		break;
	case 1:
		reg_addr = (void *)IOCFG_BM_BASE;
		break;
	case 2:
		reg_addr = (void *)IOCFG_BR_BASE;
		break;
	case 3:
		reg_addr = (void *)IOCFG_LM_BASE;
		break;
	case 4:
		reg_addr = (void *)IOCFG_RB_BASE;
		break;
	case 5:
		reg_addr = (void *)IOCFG_TL_BASE;
		break;
	default:
		reg_addr = NULL;
		break;
	}

	return reg_addr;
}
