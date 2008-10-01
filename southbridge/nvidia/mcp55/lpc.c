/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Linux Networx
 * Copyright (C) 2003 SuSE Linux AG
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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

#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <msr.h>
#include <legacy.h>
#include <device/pci_ids.h>
#include <statictree.h>
#include <config.h>
#include <mc146818rtc.h>
#include <io.h>
#include <cpu.h>
#include <lapic.h>
#include "mcp55.h"



#define NMI_OFF	0

struct ioapicreg {
	unsigned int reg;
	unsigned int value_low, value_high;
};

static struct ioapicreg ioapicregvalues[] = {
#define ALL		(0xff << 24)
#define NONE		(0)
#define DISABLED	(1 << 16)
#define ENABLED		(0 << 16)
#define TRIGGER_EDGE	(0 << 15)
#define TRIGGER_LEVEL	(1 << 15)
#define POLARITY_HIGH	(0 << 13)
#define POLARITY_LOW	(1 << 13)
#define PHYSICAL_DEST	(0 << 11)
#define LOGICAL_DEST	(1 << 11)
#define ExtINT		(7 << 8)
#define NMI		(4 << 8)
#define SMI		(2 << 8)
#define INT		(1 << 8)
	/* IO-APIC virtual wire mode configuration */
	/* mask, trigger, polarity, destination, delivery, vector */
	{   0, ENABLED | TRIGGER_EDGE | POLARITY_HIGH | PHYSICAL_DEST | ExtINT, NONE},
	{   1, DISABLED, NONE},
	{   2, DISABLED, NONE},
	{   3, DISABLED, NONE},
	{   4, DISABLED, NONE},
	{   5, DISABLED, NONE},
	{   6, DISABLED, NONE},
	{   7, DISABLED, NONE},
	{   8, DISABLED, NONE},
	{   9, DISABLED, NONE},
	{  10, DISABLED, NONE},
	{  11, DISABLED, NONE},
	{  12, DISABLED, NONE},
	{  13, DISABLED, NONE},
	{  14, DISABLED, NONE},
	{  15, DISABLED, NONE},
	{  16, DISABLED, NONE},
	{  17, DISABLED, NONE},
	{  18, DISABLED, NONE},
	{  19, DISABLED, NONE},
	{  20, DISABLED, NONE},
	{  21, DISABLED, NONE},
	{  22, DISABLED, NONE},
	{  23, DISABLED, NONE},
	/* Be careful and don't write past the end... */
};

static void setup_ioapic(unsigned long ioapic_base, int master)
{
	int i;
	unsigned long value_low, value_high;
//	unsigned long ioapic_base = 0xfec00000;
	volatile unsigned long *l;
	struct ioapicreg *a = ioapicregvalues;

	if (master) {
		ioapicregvalues[0].value_high = lapicid()<<(56-32);
		ioapicregvalues[0].value_low = ENABLED | TRIGGER_EDGE | POLARITY_HIGH | PHYSICAL_DEST | ExtINT;
	} else {
		ioapicregvalues[0].value_high = NONE;
		ioapicregvalues[0].value_low = DISABLED;
	}

	l = (unsigned long *) ioapic_base;

	for (i = 0; i < ARRAY_SIZE(ioapicregvalues);
	     i++, a++) {
		l[0] = (a->reg * 2) + 0x10;
		l[4] = a->value_low;
		value_low = l[4];
		l[0] = (a->reg *2) + 0x11;
		l[4] = a->value_high;
		value_high = l[4];
		if ((i==0) && (value_low == 0xffffffff)) {
			printk(BIOS_WARNING, "IO APIC not responding.\n");
			return;
		}
		printk(BIOS_SPEW, "for IRQ, reg 0x%08x value 0x%08x 0x%08x\n",
			    a->reg, a->value_low, a->value_high);
	}
}

// 0x7a or e3
#define PREVIOUS_POWER_STATE	0x7A

#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define SLOW_CPU_OFF		0
#define SLOW_CPU__ON		1

#ifndef MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif

static void lpc_common_init(struct device *dev, int master)
{
	u8 byte;
	u32 dword;

	/* IO APIC initialization */
	byte = pci_read_config8(dev, 0x74);
	byte |= (1<<0); // enable APIC
	pci_write_config8(dev, 0x74, byte);
	dword = pci_read_config32(dev, PCI_BASE_ADDRESS_1); // 0x14

	setup_ioapic(dword, master);
}

static void lpc_slave_init(struct device *dev)
{
	lpc_common_init(dev, 0);
}

#if CONFIG_HPET
static void enable_hpet(struct device *dev)
{
	unsigned long hpet_address;

	pci_write_config32(dev,0x44, 0xfed00001);
	hpet_address=pci_read_config32(dev,0x44)& 0xfffffffe;
	printk(BIOS_DEBUG, "enabling HPET @0x%x\n", hpet_address);
}
#endif

static void lpc_init(struct device *dev)
{
	u8 byte;
	u8 byte_old;
	int on;
	int nmi_option;

	lpc_common_init(dev, 1);

#warning posted memory write enable disabled in mcp55 lpc?
#if 0
	/* posted memory write enable */
	byte = pci_read_config8(dev, 0x46);
	pci_write_config8(dev, 0x46, byte | (1<<0));
#endif
	/* power after power fail */

#if 1
	on = MAINBOARD_POWER_ON_AFTER_POWER_FAIL;
	get_option(&on, "power_on_after_fail");
	byte = pci_read_config8(dev, PREVIOUS_POWER_STATE);
	byte &= ~0x40;
	if (!on) {
		byte |= 0x40;
	}
	pci_write_config8(dev, PREVIOUS_POWER_STATE, byte);
	printk(BIOS_INFO, "set power %s after power fail\n", on?"on":"off");
#endif
	/* Throttle the CPU speed down for testing */
	on = SLOW_CPU_OFF;
	get_option(&on, "slow_cpu");
	if(on) {
		u16 pm10_bar;
		u32 dword;
		pm10_bar = (pci_read_config16(dev, 0x60)&0xff00);
		outl(((on<<1)+0x10)  ,(pm10_bar + 0x10));
		dword = inl(pm10_bar + 0x10);
		on = 8-on;
		printk(BIOS_DEBUG, "Throttling CPU %2d.%1.1d percent.\n",
			     (on*12)+(on>>1),(on&1)*5);
	}

#if 0
// default is enabled
	/* Enable Port 92 fast reset */
	byte = pci_read_config8(dev, 0xe8);
	byte |= ~(1 << 3);
	pci_write_config8(dev, 0xe8, byte);
#endif

	/* Enable Error reporting */
	/* Set up sync flood detected */
	byte = pci_read_config8(dev, 0x47);
	byte |= (1 << 1);
	pci_write_config8(dev, 0x47, byte);

	/* Set up NMI on errors */
	byte = inb(0x70); // RTC70
	byte_old = byte;
	nmi_option = NMI_OFF;
	get_option(&nmi_option, "nmi");
	if (nmi_option) {
		byte &= ~(1 << 7); /* set NMI */
	} else {
		byte |= ( 1 << 7); // Can not mask NMI from PCI-E and NMI_NOW
	}
	if( byte != byte_old) {
		outb(0x70, byte);
	}

	/* Initialize the real time clock */
	rtc_init(0);

	/* Initialize isa dma */
	isa_dma_init();

	/* Initialize the High Precision Event Timers */
	/* not on this chip; they probably don't work. */
//	enable_hpet(dev);

}

static void mcp55_lpc_read_resources(struct device *dev)
{
	struct resource *res;

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev); // We got one for APIC, or one more for TRAP

	/* Add an extra subtractive resource for both memory and I/O */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

}

/**
 * @brief Enable resources for children devices
 *
 * @param dev the device whos children's resources are to be enabled
 *
 * This function is call by the global dev_phase5() indirectly via the
 * device_operation::phase5_enable_resources() method of devices.
 *
 * Indirect mutual recursion:
 *      phase5_children() -> dev_phase5()
 *      phase5() -> device_operation::phase5_enable_resources()
 *      device_operation::phase5_enable_resources() -> phase5_children()
 */
static void mcp55_lpc_enable_childrens_resources(struct device *dev)
{
	unsigned link;
	u32 reg, reg_var[4];
	int i;
	int var_num = 0;

	reg = pci_read_config32(dev, 0xa0);

	for (link = 0; link < dev->links; link++) {
		struct device *child;
		for (child = dev->link[link].children; child; child = child->sibling) {
			dev_phase5(child);
			if(child->have_resources && (child->path.type == DEVICE_PATH_PNP)) {
				for(i=0;i<child->resources;i++) {
					struct resource *res;
					unsigned long base, end; // don't need long long
					res = &child->resource[i];
					if(!(res->flags & IORESOURCE_IO)) continue;
					base = res->base;
					end = resource_end(res);
					printk(BIOS_DEBUG, "mcp55 lpc decode:%s, base=0x%08lx, end=0x%08lx\n",dev_path(child),base, end);
					switch(base) {
					case 0x3f8: // COM1
						reg |= (1<<0);	break;
					case 0x2f8: // COM2
						reg |= (1<<1);  break;
					case 0x378: // Parallel 1
						reg |= (1<<24); break;
					case 0x3f0: // FD0
						reg |= (1<<20); break;
					case 0x220:  // Aduio 0
						reg |= (1<<8);	break;
					case 0x300:  // Midi 0
						reg |= (1<<12);	break;
					}
					if( (base == 0x290) || (base >= 0x400)) {
						if(var_num>=4) continue; // only 4 var ; compact them ?
						reg |= (1<<(28+var_num));
						reg_var[var_num++] = (base & 0xffff)|((end & 0xffff)<<16);
					}
				}
			}
		}
	}
	pci_write_config32(dev, 0xa0, reg);
	for(i=0;i<var_num;i++) {
		pci_write_config32(dev, 0xa8 + i*4, reg_var[i]);
	}


}

static void mcp55_lpc_enable_resources(struct device *dev)
{
	pci_dev_enable_resources(dev);
	mcp55_lpc_enable_childrens_resources(dev);
}

struct device_operations mcp55_lpc = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			      .device = PCI_DEVICE_ID_NVIDIA_MCP55_LPC}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = scan_static_bus,
	.phase4_read_resources	 = mcp55_lpc_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = mcp55_lpc_enable_resources,
	.phase6_init		 = lpc_init,
	.ops_pci		 = &mcp55_pci_dev_ops_pci,
};

struct device_operations mcp55_pro = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			      .device = PCI_DEVICE_ID_NVIDIA_MCP55_PRO}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = scan_static_bus,
	.phase4_read_resources	 = mcp55_lpc_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = mcp55_lpc_enable_resources,
	.phase6_init		 = lpc_init,
	.ops_pci		 = &mcp55_pci_dev_ops_pci,
};

struct device_operations mcp55_lpc2 = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			      .device = PCI_DEVICE_ID_NVIDIA_MCP55_LPC_2}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = scan_static_bus,
	.phase4_read_resources	 = mcp55_lpc_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = mcp55_lpc_enable_resources,
	.phase6_init		 = lpc_init,
	.ops_pci		 = &mcp55_pci_dev_ops_pci,
};

struct device_operations mcp55_lpc3 = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			      .device = PCI_DEVICE_ID_NVIDIA_MCP55_LPC_3}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = scan_static_bus,
	.phase4_read_resources	 = mcp55_lpc_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = mcp55_lpc_enable_resources,
	.phase6_init		 = lpc_init,
	.ops_pci		 = &mcp55_pci_dev_ops_pci,
};

struct device_operations mcp55_lpc4 = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			      .device = PCI_DEVICE_ID_NVIDIA_MCP55_LPC_4}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = scan_static_bus,
	.phase4_read_resources	 = mcp55_lpc_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = mcp55_lpc_enable_resources,
	.phase6_init		 = lpc_init,
	.ops_pci		 = &mcp55_pci_dev_ops_pci,
};

struct device_operations mcp55_lpc5 = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			      .device = PCI_DEVICE_ID_NVIDIA_MCP55_LPC_5}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = scan_static_bus,
	.phase4_read_resources	 = mcp55_lpc_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = mcp55_lpc_enable_resources,
	.phase6_init		 = lpc_init,
	.ops_pci		 = &mcp55_pci_dev_ops_pci,
};

struct device_operations mcp55_lpc6 = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			      .device = PCI_DEVICE_ID_NVIDIA_MCP55_LPC_6}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = scan_static_bus,
	.phase4_read_resources	 = mcp55_lpc_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = mcp55_lpc_enable_resources,
	.phase6_init		 = lpc_init,
	.ops_pci		 = &mcp55_pci_dev_ops_pci,
};

struct device_operations mcp55_lpcslave = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_NVIDIA,
			      .device = PCI_DEVICE_ID_NVIDIA_MCP55_SLAVE}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = scan_static_bus,
	.phase4_read_resources	 = mcp55_lpc_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = lpc_slave_init,
	.ops_pci		 = &mcp55_pci_dev_ops_pci,
};
