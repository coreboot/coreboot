/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Rockchip Inc.
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

#include <arch/io.h>
#include <console/console.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/grf.h>
#include <soc/pmu.h>
#include <soc/soc.h>
#include <stdlib.h>

struct rockchip_gpio_regs *gpio_port[] = {
	(struct rockchip_gpio_regs *)0xff750000,
	(struct rockchip_gpio_regs *)0xff780000,
	(struct rockchip_gpio_regs *)0xff790000,
	(struct rockchip_gpio_regs *)0xff7a0000,
	(struct rockchip_gpio_regs *)0xff7b0000,
	(struct rockchip_gpio_regs *)0xff7c0000,
	(struct rockchip_gpio_regs *)0xff7d0000,
	(struct rockchip_gpio_regs *)0xff7e0000,
	(struct rockchip_gpio_regs *)0xff7f0000
};

#define PMU_GPIO_PORT 0

int is_pmu_gpio(gpio_t gpio)
{
	if (gpio.port == PMU_GPIO_PORT)
		return 1;
	return 0;
}

void *gpio_grf_reg(gpio_t gpio)
{
	if (is_pmu_gpio(gpio))
		return &rk3288_pmu->gpio0pull[gpio.bank];
	/* There is one pmu gpio, gpio0 , so " - 1" */
	return &rk3288_grf->gpio1_p[(gpio.port - 1)][gpio.bank];
}

u32 gpio_get_pull_val(gpio_t gpio, u32 pull)
{
	/* use the default gpio pull bias setting defined in soc/gpio.h */
	return pull;
}
