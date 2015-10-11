/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The Chromium OS Authors. All rights reserved.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <stdint.h>
#include <string.h>
#include <arch/io.h>

#include "soc.h"
#include "gpio.h"

#define MAX_GPIO_NUMBER 31 /* zero based */

void setup_soc_gpios(const struct soc_gpio_map *gpio)
{
	u16 gpiobase = pci_read_config16(SOC_LPC_DEV, GBASE) & ~0xf;
	u32 *cfiobase = (u32 *)(pci_read_config32(SOC_LPC_DEV, IOBASE) & ~0xf);
	u32 cfio_cnt = 0;


	/* GPIO */
	if (gpio->core.level)
		outl(*((u32*)gpio->core.level), gpiobase + GPIO_SC_GP_LVL);
	if (gpio->core.mode)
		outl(*((u32*)gpio->core.mode), gpiobase + GPIO_SC_USE_SEL);
	if (gpio->core.direction)
		outl(*((u32*)gpio->core.direction), gpiobase + GPIO_SC_IO_SEL);
	if (gpio->core.tpe)
		outl(*((u32*)gpio->core.tpe), gpiobase + GPIO_SC_TPE);
	if (gpio->core.tne)
		outl(*((u32*)gpio->core.tne), gpiobase + GPIO_SC_TNE);
	if (gpio->core.ts)
		outl(*((u32*)gpio->core.ts), gpiobase + GPIO_SC_TS);

	/* GPIO SUS Well Set 1 */
	if (gpio->sus.level)
		outl(*((u32*)gpio->sus.level), gpiobase + GPIO_SUS_GP_LVL);
	if (gpio->sus.mode)
		outl(*((u32*)gpio->sus.mode), gpiobase + GPIO_SUS_USE_SEL);
	if (gpio->sus.direction)
		outl(*((u32*)gpio->sus.direction), gpiobase + GPIO_SUS_IO_SEL);
	if (gpio->sus.tpe)
		outl(*((u32*)gpio->sus.tpe), gpiobase + GPIO_SUS_TPE);
	if (gpio->sus.tne)
		outl(*((u32*)gpio->sus.tne), gpiobase + GPIO_SUS_TNE);
	if (gpio->sus.ts)
		outl(*((u32*)gpio->sus.ts), gpiobase + GPIO_SUS_TS);
	if (gpio->sus.we)
		outl(*((u32*)gpio->sus.we), gpiobase + GPIO_SUS_WE);

	/* GPIO PAD Settings */
	/* CFIO Core Well Set 1 */
	if ((gpio->core.cfio_init != NULL) || (gpio->core.cfio_entrynum != 0)) {
		write32(cfiobase + (0x0700 / sizeof(u32)), (u32)0x01001002);
		for(cfio_cnt = 0; cfio_cnt < gpio->core.cfio_entrynum; cfio_cnt++) {
			if (!((u32)gpio->core.cfio_init[cfio_cnt].pad_conf_0))
				continue;
			write32(cfiobase + ((CFIO_PAD_CONF0 + (16*cfio_cnt))/sizeof(u32)), (u32)gpio->core.cfio_init[cfio_cnt].pad_conf_0);
			write32(cfiobase + ((CFIO_PAD_CONF1 + (16*cfio_cnt))/sizeof(u32)), (u32)gpio->core.cfio_init[cfio_cnt].pad_conf_1);
			write32(cfiobase + ((CFIO_PAD_VAL + (16*cfio_cnt))/sizeof(u32)), (u32)gpio->core.cfio_init[cfio_cnt].pad_val);
			write32(cfiobase + ((CFIO_PAD_DFT + (16*cfio_cnt))/sizeof(u32)), (u32)gpio->core.cfio_init[cfio_cnt].pad_dft);
		}
		write32(cfiobase + (0x0700 / sizeof(u32)), (u32)0x01041002);
	}

	/* CFIO SUS Well Set 1 */
	if ((gpio->sus.cfio_init != NULL) || (gpio->sus.cfio_entrynum != 0)) {
		write32(cfiobase + (0x1700 / sizeof(u32)), (u32)0x01001002);
		for(cfio_cnt = 0; cfio_cnt < gpio->sus.cfio_entrynum; cfio_cnt++) {
			if (!((u32)gpio->sus.cfio_init[cfio_cnt].pad_conf_0))
				continue;
			write32(cfiobase + ((CFIO_PAD_CONF0 + 0x1000 + (16*cfio_cnt))/sizeof(u32)), (u32)gpio->sus.cfio_init[cfio_cnt].pad_conf_0);
			write32(cfiobase + ((CFIO_PAD_CONF1 + 0x1000 + (16*cfio_cnt))/sizeof(u32)), (u32)gpio->sus.cfio_init[cfio_cnt].pad_conf_1);
			write32(cfiobase + ((CFIO_PAD_VAL + 0x1000 + (16*cfio_cnt))/sizeof(u32)), (u32)gpio->sus.cfio_init[cfio_cnt].pad_val);
			write32(cfiobase + ((CFIO_PAD_DFT + 0x1000 + (16*cfio_cnt))/sizeof(u32)), (u32)gpio->sus.cfio_init[cfio_cnt].pad_dft);
		}
		write32(cfiobase + (0x1700 / sizeof(u32)), (u32)0x01041002);
	}
}

int get_gpio(int gpio_num)
{
	u16 gpio_base = pci_read_config16(SOC_LPC_DEV, GBASE) & ~0xf;
	int bit;

	if (gpio_num > MAX_GPIO_NUMBER)
		return 0; /* Ignore wrong GPIO numbers. */

	bit = gpio_num % 32;

	return (inl(gpio_base + GPIO_SC_USE_SEL) >> bit) & 1;
}
