/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include <soc/addressmap.h>
#include <soc/grf.h>
#include <soc/soc.h>

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

#define IS_GPIO_BANK(g, p, b) (g.port == p && g.bank == GPIO_##b)

enum {
	PULLNONE_1V8 = 0,
	PULLDOWN_1V8 = 1,
	PULLUP_1V8 = 3,
};

u32 gpio_get_pull_val(gpio_t gpio, enum gpio_pull pull)
{
	/* The default pull bias setting defined in soc/gpio.h */
	u32 pull_val = pull;

	/* GPIO0_A, GPIO0_B, GPIO2_C, GPIO2_D use the 1V8 pull bias setting.
	 * Defined in TRM V.03 Part1 Page 331 and Page 458
	 */
	if (IS_GPIO_BANK(gpio, 0, A) || IS_GPIO_BANK(gpio, 0, B) ||
	    IS_GPIO_BANK(gpio, 2, C) || IS_GPIO_BANK(gpio, 2, D)) {
		switch (pull) {
		case GPIO_PULLUP:
			pull_val = PULLUP_1V8;
			break;
		case GPIO_PULLDOWN:
			pull_val = PULLDOWN_1V8;
			break;
		case GPIO_PULLNONE:
			pull_val = PULLNONE_1V8;
		}
	}

	return pull_val;
}
