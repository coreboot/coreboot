/*
 * K8 northbridge utilities (dump routing registers). Designed to be called at any time. 
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Vincent Legoll <vincent.legoll@gmail.com>
 * Original Python code
 * Convert to c (C) 2008 Ronald G. Minnich <rminnich@gmail.com>
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
 * return "R" if the register has read-enable bit set 
 */
static char *re(u32 i)
{
	if (i & 1) 
		return "R";
	else
		return "";
}

/** 
 * return "W" if the register has read-enable bit set 
 */
static char *we(u32 i)
{
	if (i & 1) 
		return "W";
	else
		return "";
}

/** 
 * return a string containing the interleave settings. 
 */
static char *ileave(u32 base)
{
	switch((base >> 8) & 7) {
		case 0: return "No interleave";
		case 1: return "2 nodes";
		case 3: return "4 nodes";
		case 7: return "8 nodes";
		default: return "Reserved";
	}
}

/**
 *  Return the node number. 
 * For one case (config registers) these are not the right bit fields. 
 */
static int node(u32 reg)
{
	return BITS(reg, 0, 7);
}

/**
 *  Return the link number. 
 * For one case (config registers) these are not the right bit fields. 
 */
static int link(u32 reg)
{
	return BITS(reg, 4, 3);
}


/**
 * Print the dram routing info for one base/limit pair. 
 * Show base, limit, dest node, dest link on that node, read and write enable, and 
 * interleave information. 
 * @param level printing level
 * @param which Register number
 * @param base Base register
 * @param limit Limit register
 */
void showdram(int level, u8 which, u32 base, u32 lim)
{
	printk(level, "DRAM(%02x)%08x-%08x, ->(%d), %s, %s, %s, %d\n", 
			which, ((base&0xfff0000)<<8), 
			((lim&0xffff0000<<8))+0xffffff, 
			node(lim), re(base), we(base), 
			ileave(base), (lim>>8)&3);
}

/**
 * Print the config routing info for a config register. 
 * Show base, limit, dest node, dest link on that node, read and write enable, and 
 * device number compare enable
 * @param level printing level
 * @param which Register number
 * @param reg config register
 */
void showconfig(int level, u8 which, u32 reg)
{
	/* don't use node() and link() here */
	printk(level, "CONFIG(%02x)%08x-%08x ->(%d,%d),%s %s CE %d\n", 
			which, BITS(reg, 24, 0xff), BITS(reg, 16, 0xff),
			BITS(reg, 4, 7), BITS(reg, 8, 3), 
			re(reg), we(reg), BITS(reg, 0, 4));
}
			
/**
 * Print the pciio routing info for one base/limit pair. 
 * Show base, limit, dest node, dest link on that node, read and write enable, and 
 * VGA and ISA Enable. 
 * @param level printing level
 * @param which Register number
 * @param base Base register
 * @param limit Limit register
 */
void showpciio(int level, u8 which, u32 base, u32 lim)
{
	printk(level, "PCIIO(%02x)%08x-%08x, ->(%d,%d), %s, %s,VGA %d ISA %d\n", 
			which, BITS(base, 12, 0x3fff), BITS(lim, 12, 0x3fff), 
			node(lim), link(lim),
			re(base), we(base), 
			BITS(base, 4, 1), BITS(base, 5, 1));
}

/**
 * Print the pciio routing info for one base/limit pair. 
 * Show base, limit, dest node, dest link on that node, read and write enable, and 
 * CPU Disable, Lock, and Non-posted. 
 * @param level printing level
 * @param which Register number
 * @param base Base register
 * @param limit Limit register
 */
void showmmio(int level, u8 which, u32 base, u32 lim)
{
	printk(level, "MMIO(%02x)%08x-%08x, ->(%d,%d), %s, %s, CPU disable %d, Lock %d, Non posted %d\n", 
			which, BITS(base, 0, 0xffffff00)<<8, 
			(BITS(lim, 0, 0xffffff00)<<8)+0xffff, 
			node(lim), link(lim), re(base), we(base), 
			BITS(base, 4, 1), 
			BITS(base, 7, 1), BITS(lim, 7, 1));

}

/**
 * Show all dram routing registers. This function is callable at any time. 
 * @param level The debug level
 * @param dev 32-bit number if the standard bus/dev/fn format which is used raw config space
 */
void showalldram(int level, u32 dev)
{
	u8 reg;
	for(reg = DRAM_ROUTE_START; reg <= DRAM_ROUTE_END; reg += 8) {
		u32 base = pci_conf1_read_config32(dev, reg);
		u32 lim = pci_conf1_read_config32(dev,reg+4);
		showdram(level, reg, base, lim);
	}		
}

/**
 * Show all mmio routing registers. This function is callable at any time. 
 * @param level The debug level
 * @param dev 32-bit number if the standard bus/dev/fn format which is used raw config space
 */
void showallmmio(int level, u32 dev)
{
	u8 reg;
	for(reg = MMIO_ROUTE_START; reg <= MMIO_ROUTE_END; reg += 8) {
		u32 base = pci_conf1_read_config32(dev, reg);
		u32 lim = pci_conf1_read_config32(dev,reg+4);
		showmmio(level, reg, base, lim);
	}		
}

/**
 * Show all pciio routing registers. This function is callable at any time. 
 * @param level The debug level
 * @param dev 32-bit number if the standard bus/dev/fn format which is used raw config space
 */
void showallpciio(int level, u32 dev)
{
	u8 reg;
	for(reg = PCIIO_ROUTE_START; reg <= PCIIO_ROUTE_END; reg += 8) {
		u32 base = pci_conf1_read_config32(dev, reg);
		u32 lim = pci_conf1_read_config32(dev,reg+4);
		showpciio(level, reg, base, lim);
	}		
}

/**
 * Show all config routing registers. This function is callable at any time. 
 * @param level The debug level
 * @param dev 32-bit number if the standard bus/dev/fn format which is used raw config space
 */
void showallconfig(int level, u32 dev)
{
	u8 reg;
	for(reg = CONFIG_ROUTE_START; reg <= CONFIG_ROUTE_END; reg += 4) {
		u32 val = pci_conf1_read_config32(dev, reg);
		showconfig(level, reg, val);
	}		
}

/**
 * Show all routing registers. This function is callable at any time. 
 * @param level The debug level
 * @param dev 32-bit number if the standard bus/dev/fn format which is used raw config space
 */
void showallroutes(int level, u32 dev)
{
	showalldram(level, dev);
	showallmmio(level, dev);
	showallpciio(level, dev);
	showallconfig(level, dev);
}
