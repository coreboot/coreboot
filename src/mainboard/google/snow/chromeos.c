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
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/ec_commands.h>
#include <string.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include <cpu/samsung/exynos5250/cpu.h>
#include <cpu/samsung/exynos5250/gpio.h>
#include <cpu/samsung/exynos5-common/gpio.h>

#define ACTIVE_LOW	0
#define ACTIVE_HIGH	1
#define WP_GPIO		6
#define DEVMODE_GPIO	54
#define RECMODE_GPIO	0
#define FORCE_RECOVERY_MODE	0
#define FORCE_DEVELOPER_MODE	0
#define LID_OPEN	5
#define POWER_BUTTON	3

#include <boot/coreboot_tables.h>

#define GPIO_COUNT	6

static struct exynos5_gpio_part1 *gpio_pt1 =
	(struct exynos5_gpio_part1 *)EXYNOS5_GPIO_PART1_BASE;
static struct exynos5_gpio_part2 *gpio_pt2 =
	(struct exynos5_gpio_part2 *)EXYNOS5_GPIO_PART2_BASE;

void fill_lb_gpios(struct lb_gpios *gpios)
{
	gpios->size = sizeof(*gpios) + (GPIO_COUNT * sizeof(struct lb_gpio));
	gpios->count = GPIO_COUNT;

	/* Write Protect: active low */
	gpios->gpios[0].port = EXYNOS5_GPD1;
	gpios->gpios[0].polarity = ACTIVE_LOW;
	gpios->gpios[0].value = s5p_gpio_get_value(&gpio_pt1->d1, WP_GPIO);
	strncpy((char *)gpios->gpios[0].name,"write protect",
							GPIO_MAX_NAME_LENGTH);

	/* Recovery: active low */
	gpios->gpios[1].port = -1;
	gpios->gpios[1].polarity = ACTIVE_HIGH;
	gpios->gpios[1].value = get_recovery_mode_switch();
	strncpy((char *)gpios->gpios[1].name,"recovery", GPIO_MAX_NAME_LENGTH);

	/* Lid: active high */
	gpios->gpios[2].port = EXYNOS5_GPX3;
	gpios->gpios[2].polarity = ACTIVE_HIGH;
	gpios->gpios[2].value = s5p_gpio_get_value(&gpio_pt2->x3, LID_OPEN);
	strncpy((char *)gpios->gpios[2].name,"lid", GPIO_MAX_NAME_LENGTH);

	/* Power: virtual GPIO active low */
	gpios->gpios[3].port = -1;
	gpios->gpios[3].polarity = ACTIVE_LOW;
	gpios->gpios[3].value = 1;
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
	uint32_t ec_events;

	/* The GPIO is active low. */
	if (!s5p_gpio_get_value(&gpio_pt1->y1, RECMODE_GPIO))
		return 1;

	ec_events = google_chromeec_get_events_b();
	return !!(ec_events &
		  EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY));
}

int get_recovery_mode_from_vbnv(void)
{
	return 1;
}
