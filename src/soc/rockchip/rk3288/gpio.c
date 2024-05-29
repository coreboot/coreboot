/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include <soc/addressmap.h>
#include <soc/grf.h>
#include <soc/pmu.h>
#include <soc/soc.h>

struct rockchip_gpio_regs *gpio_port[] = {
	(struct rockchip_gpio_regs *)GPIO0_BASE,
	(struct rockchip_gpio_regs *)GPIO1_BASE,
	(struct rockchip_gpio_regs *)GPIO2_BASE,
	(struct rockchip_gpio_regs *)GPIO3_BASE,
	(struct rockchip_gpio_regs *)GPIO4_BASE,
	(struct rockchip_gpio_regs *)GPIO5_BASE,
	(struct rockchip_gpio_regs *)GPIO6_BASE,
	(struct rockchip_gpio_regs *)GPIO7_BASE,
	(struct rockchip_gpio_regs *)GPIO8_BASE
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
	/* There is one pmu gpio, gpio0, so " - 1" */
	return &rk3288_grf->gpio1_p[(gpio.port - 1)][gpio.bank];
}

u32 gpio_get_pull_val(gpio_t gpio, enum gpio_pull pull)
{
	/* use the default gpio pull bias setting defined in soc/gpio.h */
	return pull;
}
