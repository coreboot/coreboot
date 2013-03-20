/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
#include <device/device.h>
#include <device/pci.h>
#include <southbridge/intel/bd82x6x/pch.h>

#define GPIO_SPI_WP	68
#define GPIO_REC_MODE	42
#define GPIO_DEV_MODE	17

#define FLAG_SPI_WP	0
#define FLAG_REC_MODE	1
#define FLAG_DEV_MODE	2

#ifndef __PRE_RAM__
#include <boot/coreboot_tables.h>

#define GPIO_COUNT	6
#define ACTIVE_LOW	0
#define ACTIVE_HIGH	1

void fill_lb_gpios(struct lb_gpios *gpios)
{
	device_t dev = dev_find_slot(0, PCI_DEVFN(0x1f,0));
	u16 gen_pmcon_1 = pci_read_config32(dev, GEN_PMCON_1);

	gpios->size = sizeof(*gpios) + (GPIO_COUNT * sizeof(struct lb_gpio));
	gpios->count = GPIO_COUNT;

	/* Write Protect: GPIO68 = CHP3_SPI_WP */
	gpios->gpios[0].port = GPIO_SPI_WP;
	gpios->gpios[0].polarity = ACTIVE_HIGH;
	gpios->gpios[0].value =
		(pci_read_config32(dev_find_slot(0, PCI_DEVFN(0x1f, 2)),
						 SATA_SP) >> FLAG_SPI_WP) & 1;
	strncpy((char *)gpios->gpios[0].name,"write protect",
							GPIO_MAX_NAME_LENGTH);

	/* Recovery: GPIO42 = CHP3_REC_MODE# */
	gpios->gpios[1].port = GPIO_REC_MODE;
	gpios->gpios[1].polarity = ACTIVE_LOW;
	gpios->gpios[1].value = !get_recovery_mode_switch();
	strncpy((char *)gpios->gpios[1].name,"recovery", GPIO_MAX_NAME_LENGTH);

	/* Developer: GPIO17 = KBC3_DVP_MODE */
	gpios->gpios[2].port = GPIO_DEV_MODE;
	gpios->gpios[2].polarity = ACTIVE_HIGH;
	gpios->gpios[2].value = get_developer_mode_switch();
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

	/* Did we load the VGA Option ROM? */
	gpios->gpios[5].port = -1; /* Indicate that this is a pseudo GPIO */
	gpios->gpios[5].polarity = ACTIVE_HIGH;
	gpios->gpios[5].value = oprom_is_loaded;
	strncpy((char *)gpios->gpios[5].name,"oprom", GPIO_MAX_NAME_LENGTH);
}
#endif

int get_developer_mode_switch(void)
{
	device_t dev;
#ifdef __PRE_RAM__
	dev = PCI_DEV(0, 0x1f, 2);
#else
	dev = dev_find_slot(0, PCI_DEVFN(0x1f, 2));
#endif
	return (pci_read_config32(dev, SATA_SP) >> FLAG_DEV_MODE) & 1;
}

int get_recovery_mode_switch(void)
{
	device_t dev;
#ifdef __PRE_RAM__
	dev = PCI_DEV(0, 0x1f, 2);
#else
	dev = dev_find_slot(0, PCI_DEVFN(0x1f, 2));
#endif
	return (pci_read_config32(dev, SATA_SP) >> FLAG_REC_MODE) & 1;
}

#ifdef __PRE_RAM__
void save_chromeos_gpios(void)
{
	u16 gpio_base = pci_read_config32(PCH_LPC_DEV, GPIO_BASE) & 0xfffe;
	u32 gp_lvl3 = inl(gpio_base + GP_LVL3);
	u32 gp_lvl2 = inl(gpio_base + GP_LVL2);
	u32 gp_lvl = inl(gpio_base + GP_LVL);
	u32 flags = 0;

	/* Write Protect: GPIO68 = CHP3_SPI_WP, active high */
	if (gp_lvl3 & (1 << (GPIO_SPI_WP-64)))
		flags |= (1 << FLAG_SPI_WP);
	/* Recovery: GPIO42 = CHP3_REC_MODE#, active low */
	if (!(gp_lvl2 & (1 << (GPIO_REC_MODE-32))))
		flags |= (1 << FLAG_REC_MODE);
	/* Developer: GPIO17 = KBC3_DVP_MODE, active high */
	if (gp_lvl & (1 << GPIO_DEV_MODE))
		flags |= (1 << FLAG_DEV_MODE);

	pci_write_config32(PCI_DEV(0, 0x1f, 2), SATA_SP, flags);
}
#endif
