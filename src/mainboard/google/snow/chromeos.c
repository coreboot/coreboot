/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
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

#include <console/console.h>
#include <string.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <arch/io.h>

#include <device/device.h>

#define ACTIVE_LOW	0
#define ACTIVE_HIGH	1
#define WP_GPIO		6
#define DEVMODE_GPIO	54
#define FORCE_RECOVERY_MODE	0
#define FORCE_DEVELOPER_MODE	0

#include <boot/coreboot_tables.h>
#include <arch/coreboot_tables.h>

#define GPIO_COUNT	6

void fill_lb_gpios(struct lb_gpios *gpios)
{

	gpios->size = sizeof(*gpios) + (GPIO_COUNT * sizeof(struct lb_gpio));
	gpios->count = GPIO_COUNT;

	/* Write Protect: virtual GPIO active Low */
	gpios->gpios[0].port = -1;
	gpios->gpios[0].polarity = ACTIVE_LOW;
	gpios->gpios[0].value = 1;
	strncpy((char *)gpios->gpios[0].name,"write protect",
							GPIO_MAX_NAME_LENGTH);

	/* Recovery: virtual GPIO active high */
	gpios->gpios[1].port = -1;
	gpios->gpios[1].polarity = ACTIVE_HIGH;
	gpios->gpios[1].value = 0;
	strncpy((char *)gpios->gpios[1].name,"recovery", GPIO_MAX_NAME_LENGTH);

	/* Lid: the "switch" comes from the EC */
	gpios->gpios[2].port = -1;
	gpios->gpios[2].polarity = ACTIVE_HIGH;
	gpios->gpios[2].value = 0;
	strncpy((char *)gpios->gpios[2].name,"lid", GPIO_MAX_NAME_LENGTH);

	/* Power: hardcoded as not pressed */
	gpios->gpios[3].port = -1;
	gpios->gpios[3].polarity = ACTIVE_HIGH;
	gpios->gpios[3].value = 0;
	strncpy((char *)gpios->gpios[3].name,"power", GPIO_MAX_NAME_LENGTH);

	/* Developer: virtual GPIO active high */
	gpios->gpios[4].port = -1;
	gpios->gpios[4].polarity = ACTIVE_HIGH;
	gpios->gpios[4].value = 0;
	strncpy((char *)gpios->gpios[4].name,"developer",
							GPIO_MAX_NAME_LENGTH);

	/* Was VGA Option ROM loaded? */
	gpios->gpios[5].port = -1; /* Indicate that this is a pseudo GPIO */
	gpios->gpios[5].polarity = ACTIVE_HIGH;
	gpios->gpios[5].value = 0;
	strncpy((char *)gpios->gpios[5].name,"oprom", GPIO_MAX_NAME_LENGTH);

	printk(BIOS_ERR, "Added %d GPIOS size %d\n", GPIO_COUNT, gpios->size);

}

int get_developer_mode_switch(void)
{
	int dev_mode = 0;

	printk(BIOS_DEBUG,"FORCING DEVELOPER MODE.\n");

	dev_mode = 1;
	printk(BIOS_DEBUG,"DEVELOPER MODE FROM GPIO %d: %x\n",DEVMODE_GPIO,
								 dev_mode);

	return dev_mode;
}

int get_recovery_mode_switch(void)
{
	int ec_rec_mode = 0;

	return ec_rec_mode;
}

int get_recovery_mode_from_vbnv(void)
{
	return 1;
}
