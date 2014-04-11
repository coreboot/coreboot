/*
 * Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <iomap.h>
#include <gpio.h>
#include <arch/io.h>

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
unsigned int func - Functionality number
unsigned int pull - pull up/down, no pull range(0-3)
unsigned int drvstr - range (0 - 7)-> (2- 16)MA steps of 2
unsigned int enable - 1 - Disable, 2- Enable.

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
        unsigned int *addr = (unsigned int *)GPIO_CONFIG_ADDR(gpio);
        writel(val, addr);
}

/*******************************************************
Function description: Get GPIO configuration
Arguments :
gpio_t gpio - Gpio number
unsigned int *func - Functionality number
unsigned int *pull - pull up/down, no pull range(0-3)
unsigned int *drvstr - range (0 - 7)-> (2- 16)MA steps of 2
unsigned int *enable - 1 - Disable, 2- Enable.

Return : None
*******************************************************/


void gpio_tlmm_config_get(gpio_t gpio, unsigned int *func,
			  unsigned int *pull, unsigned int *drvstr,
			  unsigned int *enable)
{
        unsigned int val;

	if (gpio_not_valid(gpio))
		return;

        unsigned int *addr = (unsigned int *)GPIO_CONFIG_ADDR(gpio);
	val = readl(addr);

	*pull = (val >> GPIO_CFG_PULL_SHIFT) & GPIO_CFG_PULL_MASK;
	*func = (val >> GPIO_CFG_FUNC_SHIFT) & GPIO_CFG_FUNC_MASK;
	*drvstr = (val >> GPIO_CFG_DRV_SHIFT) & GPIO_CFG_DRV_MASK;
	*enable = (val >> GPIO_CFG_OE_SHIFT) & GPIO_CFG_OE_MASK;
}

/*******************************************************
Function description: configure GPIO IO functinality
Arguments :
gpio_t gpio - Gpio number
unsigned int out - Controls value of GPIO output

Return : None
*******************************************************/

void gpio_io_config_set(gpio_t gpio, unsigned int out)
{
        unsigned int val;

	if (gpio_not_valid(gpio))
		return;

	unsigned int *addr = (unsigned int *)GPIO_CONFIG_ADDR(gpio);

	val = readl(addr);
	if (out)
		val |= (1 << GPIO_IO_OUT_SHIFT);
	else
		val &= (~(1 << GPIO_IO_OUT_SHIFT));

	writel(val,addr);
}

/*******************************************************
Function description: get GPIO IO functinality details
Arguments :
gpio_t gpio - Gpio number
unsigned int *in - Value of GPIO input
unsigned int *out - Value of GPIO output

Return : None
*******************************************************/

void gpio_io_config_get(gpio_t gpio, unsigned int *in, unsigned int *out)
{
        unsigned int val;

	if (gpio_not_valid(gpio))
		return;

	unsigned int *addr = (unsigned int *)GPIO_CONFIG_ADDR(gpio);

	val = readl(addr);
	*in = (val >> GPIO_IO_IN_SHIFT)  & GPIO_IO_IN_MASK;
	*out = (val >> GPIO_IO_OUT_SHIFT) & GPIO_IO_OUT_MASK;
}
