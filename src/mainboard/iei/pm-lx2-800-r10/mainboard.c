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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/device.h>
#include <boot/tables.h>
#include <pc80/mc146818rtc.h>
#include "chip.h"

#if CONFIG_USE_OPTION_TABLE
#	include "option_table.h"
#endif

/* SCH3114 runtime register (RTR) address. */
#define SCH3114_RTR_ADDR	(0x400)
/* SP34 option register. */
#define SCH3114_RTR_SP34_OPT	(SCH3114_RTR_ADDR + 0x13)
/* H/W Monitoring register block index. */
#define SCH3114_RTR_HWM_IDX	(SCH3114_RTR_ADDR + 0x70)
/* H/W Monitoring register block data. */
#define SCH3114_RTR_HWM_DAT	(SCH3114_RTR_ADDR + 0x71)
/* H/W Monitoring Ready/Lock/Start register. */
#define SCH3114_HWM_RLS_REG	(0x40)
/* IT8888 PCI Vendor ID. */
#define IT8888_PCI_VID		(0x1283)
/* IT8888 PCI Device ID. */
#define IT8888_PCI_DID		(0x8888)

static u16 opt(const char* option, u16 def_value)
{
#if CONFIG_USE_OPTION_TABLE
	u16 value = 0;
	if (get_option(&value, option) != 0)
		value = def_value;
	return value;
#else
	return def_value;
#endif
}

static void init(struct device *dev)
{
	/* SCH3114: enable hardware monitor. */
	printk(BIOS_INFO, "Enabling SCH3114 hardware monitor\n");
	outb(SCH3114_HWM_RLS_REG, SCH3114_RTR_HWM_IDX);
	outb(inb(SCH3114_RTR_HWM_DAT) | 0x01, SCH3114_RTR_HWM_DAT);

	/* SCH3114: COM3 Automatic Direction Control / nRTS control. */
	if (opt("com3_auto_direction", CONFIG_SCH3114_COM3_AUTO_DIR) != 0) {
		printk(BIOS_INFO, "Enabling RS858 Auto Direction Control\n");
		outb(inb(SCH3114_RTR_SP34_OPT) | 0x03, SCH3114_RTR_SP34_OPT);
	}

	/* IT8888. */
	device_t it8x = dev_find_device(IT8888_PCI_VID, IT8888_PCI_DID, 0);
	if (!it8x) {
		printk(BIOS_ERR, "IT8888 not found\n");
		return;
	}

	u32 cfg = 0;
#define IT8888_IO(name, reg, dena, dspeed, dsize, daddr)	\
	if (opt(name, dena) != 0) {				\
		cfg = (1 << 31);				\
		cfg |= opt(name "_addr", daddr);		\
		cfg |= opt(name "_speed", dspeed) << 29;	\
		cfg |= opt(name "_size", dsize) << 24;		\
	} else {						\
		cfg = 0;					\
	}							\
	pci_write_config32(it8x, reg, cfg);

#include "it8888_io.def"
}

static void enable_dev(struct device *dev)
{
	dev->ops->init = init;
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("IEI PM-LX2-800-R10 Mainboard")
	.enable_dev = enable_dev,
};
