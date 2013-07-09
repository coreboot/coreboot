/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.  All rights reserved.
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

#include <arch/gpio.h>
#include <arch/hlt.h>
#include <console/console.h>
#include <cpu/samsung/exynos5250/gpio.h>
#include <cpu/samsung/exynos5250/power.h>
#include <cpu/samsung/exynos5250/exynos5-common.h>

#include <cpu/samsung/exynos5250/wakeup.h>

int wakeup_need_reset(void)
{
	/* The "wake up" event is not reliable (known as "bad wakeup") and needs
	 * reset if GPIO value is high. */
	return gpio_get_value(GPIO_Y10);
}

