/* SPDX-License-Identifier: BSD-3-Clause */

#include <device/mmio.h>
#include <gpio.h>
#include <soc/iomap.h>

/*******************************************************
Function description: check for invalid GPIO #
Arguments :
gpio_t gpio - Gpio number

Return : GPIO Valid(0)/Invalid(1)
*******************************************************/

static inline int gpio_not_valid(gpio_t gpio)
{
	return (gpio > GPIO_MAX_NUM);
}

/*******************************************************
Function description: configure GPIO functinality
Arguments :
gpio_t gpio - Gpio number
unsigned func - Functionality number
unsigned pull - pull up/down, no pull range(0-3)
unsigned drvstr - range (0 - 7)-> (2- 16)MA steps of 2
unsigned enable - 0 Disable, 1 - Enable.

Return : None
*******************************************************/

void gpio_tlmm_config_set(gpio_t gpio, unsigned int func,
			  unsigned int pull, unsigned int drvstr,
			  unsigned int enable)
{
	unsigned int val = 0;

	if (gpio_not_valid(gpio))
		return;

	val |= (pull & GPIO_CFG_PULL_MASK) << GPIO_CFG_PULL_SHIFT;
	val |= (func & GPIO_CFG_FUNC_MASK) << GPIO_CFG_FUNC_SHIFT;
	val |= (drvstr & GPIO_CFG_DRV_MASK) << GPIO_CFG_DRV_SHIFT;
	val |= (enable & GPIO_CFG_OE_MASK) << GPIO_CFG_OE_SHIFT;

	write32(GPIO_CONFIG_ADDR(gpio), val);
}

/*******************************************************
Function description: Get GPIO configuration
Arguments :
gpio_t gpio - Gpio number
unsigned *func - Functionality number
unsigned *pull - pull up/down, no pull range(0-3)
unsigned *drvstr - range (0 - 7)-> (2- 16)MA steps of 2
unsigned *enable - 0 - Disable, 1- Enable.

Return : None
*******************************************************/

void gpio_tlmm_config_get(gpio_t gpio, unsigned int *func,
			  unsigned int *pull, unsigned int *drvstr,
			  unsigned int *enable)
{
	unsigned int val;
	void *addr = GPIO_CONFIG_ADDR(gpio);

	if (gpio_not_valid(gpio))
		return;

	val = read32(addr);

	*pull = (val >> GPIO_CFG_PULL_SHIFT) & GPIO_CFG_PULL_MASK;
	*func = (val >> GPIO_CFG_FUNC_SHIFT) & GPIO_CFG_FUNC_MASK;
	*drvstr = (val >> GPIO_CFG_DRV_SHIFT) & GPIO_CFG_DRV_MASK;
	*enable = (val >> GPIO_CFG_OE_SHIFT) & GPIO_CFG_OE_MASK;
}

/*******************************************************
Function description: get GPIO IO functinality details
Arguments :
gpio_t gpio - Gpio number
unsigned *in - Value of GPIO input
unsigned *out - Value of GPIO output

Return : None
*******************************************************/
int gpio_get(gpio_t gpio)
{
	if (gpio_not_valid(gpio))
		return -1;

	return (read32(GPIO_IN_OUT_ADDR(gpio)) >> GPIO_IO_IN_SHIFT) &
		GPIO_IO_IN_MASK;
}

void gpio_set(gpio_t gpio, int value)
{
	if (gpio_not_valid(gpio))
		return;

	write32(GPIO_IN_OUT_ADDR(gpio), (value & 1) << GPIO_IO_OUT_SHIFT);
}

void gpio_input_pulldown(gpio_t gpio)
{
	gpio_tlmm_config_set(gpio, GPIO_FUNC_DISABLE,
			     GPIO_PULL_DOWN, GPIO_2MA, GPIO_DISABLE);
}

void gpio_input_pullup(gpio_t gpio)
{
	gpio_tlmm_config_set(gpio, GPIO_FUNC_DISABLE,
			     GPIO_PULL_UP, GPIO_2MA, GPIO_DISABLE);
}

void gpio_input(gpio_t gpio)
{
	gpio_tlmm_config_set(gpio, GPIO_FUNC_DISABLE,
			     GPIO_NO_PULL, GPIO_2MA, GPIO_DISABLE);
}
