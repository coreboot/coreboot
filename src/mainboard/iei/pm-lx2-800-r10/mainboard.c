/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Ricardo Martins <rasmartins@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include <arch/io.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/device.h>
#include <boot/tables.h>

/* SCH3114 runtime register (RTR) address. */
#define SCH3114_RTR_ADDR	(0x400)
/* H/W Monitoring register block index. */
#define SCH3114_RTR_HWM_IDX	(SCH3114_RTR_ADDR + 0x70)
/* H/W Monitoring register block data. */
#define SCH3114_RTR_HWM_DAT	(SCH3114_RTR_ADDR + 0x71)
/* H/W Monitoring Ready/Lock/Start register. */
#define SCH3114_HWM_RLS_REG	(0x40)

static void init(struct device *dev)
{
	/* SCH3114: enable hardware monitor. */
	printk(BIOS_INFO, "Enabling SCH3114 hardware monitor\n");
	outb(SCH3114_HWM_RLS_REG, SCH3114_RTR_HWM_IDX);
	outb(inb(SCH3114_RTR_HWM_DAT) | 0x01, SCH3114_RTR_HWM_DAT);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
