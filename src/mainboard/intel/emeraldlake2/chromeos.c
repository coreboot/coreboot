/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The Chromium OS Authors. All rights reserved.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <string.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <arch/io.h>
#ifdef __PRE_RAM__
#include <arch/romcc_io.h>
#else
#include <device/device.h>
#include <device/pci.h>
#endif
#include <southbridge/intel/bd82x6x/pch.h>

#ifndef __PRE_RAM__
#include <boot/coreboot_tables.h>
#include <arch/coreboot_tables.h>

#define GPIO_COUNT	5
#define ACTIVE_LOW	0
#define ACTIVE_HIGH	1

void fill_lb_gpios(struct lb_gpios *gpios)
{
	device_t dev = dev_find_slot(0, PCI_DEVFN(0x1f,0));
	u16 gpio_base = pci_read_config32(dev, GPIOBASE) & 0xfffe;
	u16 gen_pmcon_1 = pci_read_config32(dev, GEN_PMCON_1);

	if (!gpio_base)
		return;

#if 0 // Dev mode is hardcoded on, so we don't need to read these GPIOs.
	u32 gp_lvl = inl(gpio_base + 0x0c);
#endif
	u32 gp_lvl2 = inl(gpio_base + 0x38);
	u32 gp_lvl3 = inl(gpio_base + 0x48);

	gpios->size = sizeof(*gpios) + (GPIO_COUNT * sizeof(struct lb_gpio));
	gpios->count = GPIO_COUNT;

	/* Write Protect: GPIO68 = CHP3_SPI_WP */
	gpios->gpios[0].port = 68;
	gpios->gpios[0].polarity = ACTIVE_HIGH;
	gpios->gpios[0].value = (gp_lvl3 >> (68-64)) & 1;
	strncpy((char *)gpios->gpios[0].name,"write protect",
							GPIO_MAX_NAME_LENGTH);

	/* Recovery: GPIO42 = CHP3_REC_MODE# */
	gpios->gpios[1].port = 42;
	gpios->gpios[1].polarity = ACTIVE_LOW;
	gpios->gpios[1].value = (gp_lvl2 >> (42-32)) & 1;
	strncpy((char *)gpios->gpios[1].name,"recovery", GPIO_MAX_NAME_LENGTH);

	/* Developer: GPIO17 = KBC3_DVP_MODE */
	gpios->gpios[2].port = 17;
	gpios->gpios[2].polarity = ACTIVE_HIGH;
#if 0 // Dev mode is hardcoded on.
	gpios->gpios[2].value = (gp_lvl >> 17) & 1;
#else
	gpios->gpios[2].value = 1;
#endif
	strncpy((char *)gpios->gpios[2].name,"developer", GPIO_MAX_NAME_LENGTH);

	/* Hard code the lid switch GPIO to open. */
	gpios->gpios[3].port = 100;
	gpios->gpios[3].polarity = ACTIVE_HIGH;
	gpios->gpios[3].value = 1;
	strncpy((char *)gpios->gpios[3].name,"lid", GPIO_MAX_NAME_LENGTH);

	/* Power Button */
	gpios->gpios[4].port = 101;
	gpios->gpios[4].polarity = ACTIVE_LOW;
	gpios->gpios[4].value = (gen_pmcon_1 >> 9) & 1;
	strncpy((char *)gpios->gpios[4].name,"power", GPIO_MAX_NAME_LENGTH);
}
#endif

int get_developer_mode_switch(void)
{
#if 0 // Dev mode is hardcoded on.
	device_t dev;
#ifdef __PRE_RAM__
	dev = PCI_DEV(0, 0x1f, 0);
#else
	dev = dev_find_slot(0, PCI_DEVFN(0x1f,0));
#endif
	u16 gpio_base = pci_read_config32(dev, GPIOBASE) & 0xfffe;
	u32 gp_lvl = inl(gpio_base + 0x0c);

	/* Developer: GPIO17 = KBC3_DVP_MODE, active high */
	return (gp_lvl >> 17) & 1;
#else
	return 1;
#endif
}

int get_recovery_mode_switch(void)
{
	device_t dev;
#ifdef __PRE_RAM__
	dev = PCI_DEV(0, 0x1f, 0);
#else
	dev = dev_find_slot(0, PCI_DEVFN(0x1f,0));
#endif
	u16 gpio_base = pci_read_config32(dev, GPIOBASE) & 0xfffe;
	u32 gp_lvl2 = inl(gpio_base + 0x38);

	/* Recovery: GPIO42 = CHP3_REC_MODE#, active low */
	return !((gp_lvl2 >> (42-32)) & 1);
}

