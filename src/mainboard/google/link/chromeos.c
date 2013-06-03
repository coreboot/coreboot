/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
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
#include "ec.h"
#include <ec/google/chromeec/ec.h>

#ifndef __PRE_RAM__
#include <boot/coreboot_tables.h>

#define GPIO_COUNT	6
#define ACTIVE_LOW	0
#define ACTIVE_HIGH	1

static int get_lid_switch(void)
{
	u8 ec_switches = inb(EC_LPC_ADDR_MEMMAP + EC_MEMMAP_SWITCHES);

	return !!(ec_switches & EC_SWITCH_LID_OPEN);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	device_t dev = dev_find_slot(0, PCI_DEVFN(0x1f,0));
	u16 gpio_base = pci_read_config32(dev, GPIOBASE) & 0xfffe;
	//u16 gen_pmcon_1 = pci_read_config32(dev, GEN_PMCON_1);

	if (!gpio_base)
		return;

	u32 gp_lvl2 = inl(gpio_base + 0x38);

	gpios->size = sizeof(*gpios) + (GPIO_COUNT * sizeof(struct lb_gpio));
	gpios->count = GPIO_COUNT;

	/* Write Protect: GPIO57 = PCH_SPI_WP_D */
	gpios->gpios[0].port = 57;
	gpios->gpios[0].polarity = ACTIVE_HIGH;
	gpios->gpios[0].value = (gp_lvl2 >> (57 - 32)) & 1;
	strncpy((char *)gpios->gpios[0].name,"write protect",
							GPIO_MAX_NAME_LENGTH);
	/* Recovery: the "switch" comes from the EC */
	gpios->gpios[1].port = -1; /* Indicate that this is a pseudo GPIO */
	gpios->gpios[1].polarity = ACTIVE_HIGH;
	gpios->gpios[1].value = get_recovery_mode_switch();
	strncpy((char *)gpios->gpios[1].name,"recovery", GPIO_MAX_NAME_LENGTH);

	/* Lid: the "switch" comes from the EC */
	gpios->gpios[2].port = -1;
	gpios->gpios[2].polarity = ACTIVE_HIGH;
	gpios->gpios[2].value = get_lid_switch();
	strncpy((char *)gpios->gpios[2].name,"lid", GPIO_MAX_NAME_LENGTH);

	/* Power Button: hard-coded as not pressed; we'll detect later presses
	 * via SMI. */
	gpios->gpios[3].port = -1;
	gpios->gpios[3].polarity = ACTIVE_HIGH;
	gpios->gpios[3].value = 0;
	strncpy((char *)gpios->gpios[3].name,"power", GPIO_MAX_NAME_LENGTH);

	/* Developer: a tricky case on Link, there is no switch */
	gpios->gpios[4].port = -1; /* Indicate that this is a pseudo GPIO */
	gpios->gpios[4].polarity = ACTIVE_HIGH;
	gpios->gpios[4].value = get_developer_mode_switch();
	strncpy((char *)gpios->gpios[4].name,"developer", GPIO_MAX_NAME_LENGTH);

	/* Did we load the VGA Option ROM? */
	gpios->gpios[5].port = -1; /* Indicate that this is a pseudo GPIO */
	gpios->gpios[5].polarity = ACTIVE_HIGH;
	gpios->gpios[5].value = oprom_is_loaded;
	strncpy((char *)gpios->gpios[5].name,"oprom", GPIO_MAX_NAME_LENGTH);
}
#endif

/* The dev-switch is virtual on Link (and so handled elsewhere). */
int get_developer_mode_switch(void)
{
	return 0;
}

/* There are actually two recovery switches. One is the magic keyboard chord,
 * the other is driven by Servo. */
int get_recovery_mode_switch(void)
{
	u8 ec_switches = inb(EC_LPC_ADDR_MEMMAP + EC_MEMMAP_SWITCHES);
	u32 ec_events;

	/* If a switch is set, we don't need to look at events. */
	if (ec_switches & (EC_SWITCH_DEDICATED_RECOVERY))
		return 1;

	/* Else check if the EC has posted the keyboard recovery event. */
	ec_events = google_chromeec_get_events_b();

	return !!(ec_events &
		  EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY));
}
