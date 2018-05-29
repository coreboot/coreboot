/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Vincent Legoll <vincent.legoll@gmail.com>
 * Copyright (C) 2008 Ronald G. Minnich <rminnich@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * K8 northbridge utilities (dump routing registers).
 * Designed to be called at any time.
 * It can be called before RAM is set up by including this file.
 * It can be called after RAM is set up by including amdk8.h and enabling the
 * compilation of this file in src/northbridge/amd/amdk8/Makefile.inc.
 */
#ifndef __PRE_RAM__
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#endif
#include "amdk8.h"

/* Function 1 */
/* the DRAM, MMIO,and PCIIO routing are 64-bit registers, hence the ending at
 * 0x78, 0xb8, and 0xd8
 */
#define DRAM_ROUTE_START	0x40
#define DRAM_ROUTE_END		0x78
#define MMIO_ROUTE_START	0x80
#define MMIO_ROUTE_END		0xb8
#define PCIIO_ROUTE_START	0xc0
#define PCIIO_ROUTE_END		0xd8
#define CONFIG_ROUTE_START	0xe0
#define CONFIG_ROUTE_END	0xec

#define PCI_IO_BASE0		0xc0
#define PCI_IO_BASE1		0xc8
#define PCI_IO_BASE2		0xd0
#define PCI_IO_BASE3		0xd8
#define PCI_IO_BASE_VGA_EN	(1 << 4)
#define PCI_IO_BASE_NO_ISA	(1 << 5)

#define BITS(r, shift, mask) (((r>>shift)&mask))

/**
 * Return "R" if the register has read-enable bit set.
 */
static const char *re(u32 i)
{
	return ((i & 1) ? "R" : "");
}

/**
 * Return "W" if the register has write-enable bit set.
 */
static const char *we(u32 i)
{
	return ((i & 1) ? "W" : "");
}

/**
 * Return a string containing the interleave settings.
 */
static const char *ileave(u32 base)
{
	switch ((base >> 8) & 7) {
	case 0:
		return "No interleave";
	case 1:
		return "2 nodes";
	case 3:
		return "4 nodes";
	case 7:
		return "8 nodes";
	default:
		return "Reserved";
	}
}

/**
 * Return the node number.
 * For one case (config registers) these are not the right bit fields.
 */
static int r_node(u32 reg)
{
	return BITS(reg, 0, 0x7);
}

/**
 * Return the link number.
 * For one case (config registers) these are not the right bit fields.
 */
static int r_link(u32 reg)
{
	return BITS(reg, 4, 0x3);
}

/**
 * Print the DRAM routing info for one base/limit pair.
 *
 * Show base, limit, dest node, dest link on that node, read and write
 * enable, and interleave information.
 *
 * @param level Printing level
 * @param which Register number
 * @param base Base register
 * @param lim Limit register
 */
static void showdram(int level, u8 which, u32 base, u32 lim)
{
	printk(level, "DRAM(%02x)%010llx-%010llx, ->(%d), %s, %s, %s, %d\n",
	       which, (((u64) base & 0xffff0000) << 8),
	       (((u64) lim & 0xffff0000) << 8) + 0xffffff,
	       r_node(lim), re(base), we(base), ileave(base), (lim >> 8) & 3);
}

/**
 * Print the config routing info for a config register.
 *
 * Show base, limit, dest node, dest link on that node, read and write
 * enable, and device number compare enable
 *
 * @param level Printing level
 * @param which Register number
 * @param reg Config register
 */
static void showconfig(int level, u8 which, u32 reg)
{
	/* Don't use r_node() and r_link() here. */
	printk(level, "CONFIG(%02x)%02x-%02x ->(%d,%d),%s %s (%s numbers)\n",
	       which, BITS(reg, 16, 0xff), BITS(reg, 24, 0xff),
	       BITS(reg, 4, 0x7), BITS(reg, 8, 0x3),
	       re(reg), we(reg),
	       BITS(reg, 2, 0x1)?"dev":"bus");
}

/**
 * Print the PCIIO routing info for one base/limit pair.
 *
 * Show base, limit, dest node, dest link on that node, read and write
 * enable, and VGA and ISA Enable.
 *
 * @param level Printing level
 * @param which Register number
 * @param base Base register
 * @param lim Limit register
 */
static void showpciio(int level, u8 which, u32 base, u32 lim)
{
	printk(level, "PCIIO(%02x)%07x-%07x, ->(%d,%d), %s, %s,VGA %d ISA %d\n",
	       which, BITS(base, 12, 0x3fff) << 12,
	       (BITS(lim, 12, 0x3fff) << 12) + 0xfff, r_node(lim), r_link(lim),
	       re(base), we(base), BITS(base, 4, 0x1), BITS(base, 5, 0x1));
}

/**
 * Print the MMIO routing info for one base/limit pair.
 *
 * Show base, limit, dest node, dest link on that node, read and write
 * enable, and CPU Disable, Lock, and Non-posted.
 *
 * @param level Printing level
 * @param which Register number
 * @param base Base register
 * @param lim Limit register
 */
static void showmmio(int level, u8 which, u32 base, u32 lim)
{
	printk(level, "MMIO(%02x)%010llx-%010llx, ->(%d,%d), %s, %s, "
	       "CPU disable %d, Lock %d, Non posted %d\n",
	       which, ((u64) BITS(base, 0, 0xffffff00)) << 8,
	       (((u64) BITS(lim, 0, 0xffffff00)) << 8) + 0xffff, r_node(lim),
	       r_link(lim), re(base), we(base), BITS(base, 4, 0x1),
	       BITS(base, 7, 0x1), BITS(lim, 7, 0x1));
}

/**
 * Show all DRAM routing registers. This function is callable at any time.
 *
 * @param level The debug level.
 * @param dev A 32-bit number in the standard bus/dev/fn format which is used
 *            raw config space.
 */
static void showalldram(int level, struct device *dev)
{
	u8 reg;
	for (reg = DRAM_ROUTE_START; reg <= DRAM_ROUTE_END; reg += 8) {
		u32 base = pci_read_config32(dev, reg);
		u32 lim = pci_read_config32(dev, reg + 4);
		if (base || lim!=(reg-DRAM_ROUTE_START)/8)
			showdram(level, reg, base, lim);
	}
}

/**
 * Show all MMIO routing registers. This function is callable at any time.
 *
 * @param level The debug level.
 * @param dev A 32-bit number in the standard bus/dev/fn format which is used
 *            raw config space.
 */
static void showallmmio(int level, struct device *dev)
{
	u8 reg;
	for (reg = MMIO_ROUTE_START; reg <= MMIO_ROUTE_END; reg += 8) {
		u32 base = pci_read_config32(dev, reg);
		u32 lim = pci_read_config32(dev, reg + 4);
		if (base || lim)
			showmmio(level, reg, base, lim);
	}
}

/**
 * Show all PCIIO routing registers. This function is callable at any time.
 *
 * @param level The debug level.
 * @param dev A 32-bit number in the standard bus/dev/fn format which is used
 *            raw config space.
 */
static void showallpciio(int level, struct device *dev)
{
	u8 reg;
	for (reg = PCIIO_ROUTE_START; reg <= PCIIO_ROUTE_END; reg += 8) {
		u32 base = pci_read_config32(dev, reg);
		u32 lim = pci_read_config32(dev, reg + 4);
		if (base || lim)
			showpciio(level, reg, base, lim);
	}
}

/**
 * Show all config routing registers. This function is callable at any time.
 *
 * @param level The debug level.
 * @param dev A 32-bit number in the standard bus/dev/fn format which is used
 *            raw config space.
 */
static void showallconfig(int level, struct device *dev)
{
	u8 reg;
	for (reg = CONFIG_ROUTE_START; reg <= CONFIG_ROUTE_END; reg += 4) {
		u32 val = pci_read_config32(dev, reg);
		if (val)
			showconfig(level, reg, val);
	}
}

/**
 * Show all routing registers. This function is callable at any time.
 *
 * @param level The debug level.
 * @param dev A 32-bit number in the standard bus/dev/fn format which is used
 *            raw config space.
 */
void showallroutes(int level, struct device *dev)
{
	showalldram(level, dev);
	showallmmio(level, dev);
	showallpciio(level, dev);
	showallconfig(level, dev);
}
