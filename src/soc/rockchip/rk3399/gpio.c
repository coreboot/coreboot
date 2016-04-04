/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
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
#include <soc/addressmap.h>
#include <soc/gpio.h>
#include <soc/grf.h>
#include <soc/soc.h>
#include <stdlib.h>

struct rockchip_gpio_regs *gpio_port[] = {
	(struct rockchip_gpio_regs *)GPIO0_BASE,
	(struct rockchip_gpio_regs *)GPIO1_BASE,
	(struct rockchip_gpio_regs *)GPIO2_BASE,
	(struct rockchip_gpio_regs *)GPIO3_BASE,
	(struct rockchip_gpio_regs *)GPIO4_BASE,
};

#define PMU_GPIO_PORT0 0
#define PMU_GPIO_PORT1 1

int is_pmu_gpio(gpio_t gpio)
{
	if (gpio.port == PMU_GPIO_PORT0 || gpio.port == PMU_GPIO_PORT1)
		return 1;
	return 0;
}

void *gpio_grf_reg(gpio_t gpio)
{
	if (is_pmu_gpio(gpio))
		return &rk3399_pmugrf->gpio0_p[gpio.port][gpio.bank];
	/* There are two pmu gpio, 0 and 1, so " - 2" */
	return &rk3399_grf->gpio2_p[(gpio.port - 2)][gpio.bank];
}
