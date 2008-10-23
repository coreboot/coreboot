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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

/*
 * K8 northbridge utilities (dump routing registers).
 * Designed to be called at any time.
 */

#include <mainboard.h>
#include <console.h>
#include <string.h>
#include <mtrr.h>
#include <macros.h>
#include <spd_ddr2.h>
#include <cpu.h>
#include <msr.h>
#include <amd/k8/k8.h>
#include <amd/k8/sysconf.h>
#include <device/pci.h>
#include <pci_ops.h>
#include <mc146818rtc.h>
#include <lib.h>

#define BITS(r, shift, mask) (((r>>shift)&mask))

/**
 * Return "R" if the register has read-enable bit set.
 */
static char *re(u32 i)
{
	return ((i & 1) ? "R" : "");
}

/**
 * Return "W" if the register has read-enable bit set.
 * TODO: Shouldn't this be "write-enable"?
 */
static char *we(u32 i)
{
	return ((i & 1) ? "W" : "");
}

/**
 * Return a string containing the interleave settings.
 */
static char *ileave(u32 base)
{
	/* TODO: Are these fallthroughs intentional? */
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
static int node(u32 reg)
{
	return BITS(reg, 0, 0x7);
}

/**
 * Return the link number.
 * For one case (config registers) these are not the right bit fields.
 */
static int link(u32 reg)
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
void showdram(int level, u8 which, u32 base, u32 lim)
{
	printk(level, "DRAM(%02x)%010llx-%010llx, ->(%d), %s, %s, %s, %d\n",
	       which, (((u64) base & 0xffff0000) << 8),
	       (((u64) lim & 0xffff0000) << 8) + 0xffffff,
	       node(lim), re(base), we(base), ileave(base), (lim >> 8) & 3);
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
void showconfig(int level, u8 which, u32 reg)
{
	/* Don't use node() and link() here. */
	printk(level, "CONFIG(%02x)%02x-%02x ->(%d,%d),%s %s CE %d\n",
	       which, BITS(reg, 24, 0xff), BITS(reg, 16, 0xff),
	       BITS(reg, 4, 0x7), BITS(reg, 8, 0x3),
	       re(reg), we(reg), BITS(reg, 2, 0x1));
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
void showpciio(int level, u8 which, u32 base, u32 lim)
{
	printk(level, "PCIIO(%02x)%07x-%07x, ->(%d,%d), %s, %s,VGA %d ISA %d\n",
	       which, BITS(base, 12, 0x3fff) << 12,
	       (BITS(lim, 12, 0x3fff) << 12) + 0xfff, node(lim), link(lim),
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
void showmmio(int level, u8 which, u32 base, u32 lim)
{
	printk(level, "MMIO(%02x)%010llx-%010llx, ->(%d,%d), %s, %s, "
	       "CPU disable %d, Lock %d, Non posted %d\n",
	       which, ((u64) BITS(base, 0, 0xffffff00)) << 8,
	       (((u64) BITS(lim, 0, 0xffffff00)) << 8) + 0xffff, node(lim),
	       link(lim), re(base), we(base), BITS(base, 4, 0x1),
	       BITS(base, 7, 0x1), BITS(lim, 7, 0x1));
}

/**
 * Show all DRAM routing registers. This function is callable at any time.
 *
 * @param level The debug level.
 * @param dev A 32-bit number in the standard bus/dev/fn format which is used
 *            raw config space.
 */
void showalldram(int level, u32 dev)
{
	u8 reg;
	for (reg = DRAM_ROUTE_START; reg <= DRAM_ROUTE_END; reg += 8) {
		u32 base = pci_conf1_read_config32(dev, reg);
		u32 lim = pci_conf1_read_config32(dev, reg + 4);
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
void showallmmio(int level, u32 dev)
{
	u8 reg;
	for (reg = MMIO_ROUTE_START; reg <= MMIO_ROUTE_END; reg += 8) {
		u32 base = pci_conf1_read_config32(dev, reg);
		u32 lim = pci_conf1_read_config32(dev, reg + 4);
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
void showallpciio(int level, u32 dev)
{
	u8 reg;
	for (reg = PCIIO_ROUTE_START; reg <= PCIIO_ROUTE_END; reg += 8) {
		u32 base = pci_conf1_read_config32(dev, reg);
		u32 lim = pci_conf1_read_config32(dev, reg + 4);
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
void showallconfig(int level, u32 dev)
{
	u8 reg;
	for (reg = CONFIG_ROUTE_START; reg <= CONFIG_ROUTE_END; reg += 4) {
		u32 val = pci_conf1_read_config32(dev, reg);
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
void showallroutes(int level, u32 dev)
{
	showalldram(level, dev);
	showallmmio(level, dev);
	showallpciio(level, dev);
	showallconfig(level, dev);
}
