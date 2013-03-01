/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

#include <delay.h>
#include <stdlib.h>
#include <string.h>
#include <arch/io.h>

#include <device/device.h>

static void exynos_displayport_init(void)
{
    //int width=640, height=480, depth=8;

    printk(BIOS_DEBUG, "Initializing exynos VGA\n");

}

static void exynos_displayport_noop(device_t dummy)
{
}

static struct device_operations exynos_displayport_operations  = {
        .read_resources   = exynos_displayport_noop,
        .set_resources    = exynos_displayport_noop,
        .enable_resources = exynos_displayport_noop,
        .init             = exynos_displayport_init,
        .scan_bus         = exynos_displayport_noop,
};

static void exynos_displayport_enable(struct device *dev)
{
	if (dev->link_list != NULL)
		dev->ops = &exynos_displayport_operations;
}

struct chip_operations drivers_i2c_exynos_displayport_ops = {
	CHIP_NAME("exynos displayport")
	.enable_dev = exynos_displayport_enable;
};
