/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Linux NetworX, SuSE Linux AG
 * Copyright (C) 2006 AMD
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
#include <lapic.h> 
#include <mc146818rtc.h>
#include "amd8111.h"

#define NMI_OFF 0

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

static void setup_ioapic(void)
{
	int i;
	unsigned long value_low, value_high;
	unsigned long ioapic_base = 0xfec00000;
	volatile unsigned long *l;
	struct ioapicreg *a = ioapicregvalues;
	unsigned long bsp_apicid = lapicid();

	l = (unsigned long *) ioapic_base;

	ioapicregvalues[0].value_high = bsp_apicid<<(56-32);
	printk(BIOS_DEBUG, "amd8111: ioapic bsp_apicid = %02lx\n", bsp_apicid); 
	
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

static void enable_hpet(struct device *dev)
{
	unsigned long hpet_address;
	
	pci_write_config32(dev,0xa0, 0xfed00001);
	hpet_address = pci_read_config32(dev,0xa0)& 0xfffffffe;
	printk(BIOS_DEBUG, "enabling HPET @0x%lx\n", hpet_address);
	
}

static void lpc_init(struct device *dev)
{
	u8 byte;
	int nmi_option;

	/* IO APIC initialization */
	byte = pci_read_config8(dev, 0x4B);
	byte |= 1;
	pci_write_config8(dev, 0x4B, byte);
	setup_ioapic();

	/* posted memory write enable */
	byte = pci_read_config8(dev, 0x46);
	pci_write_config8(dev, 0x46, byte | (1<<0)); 

	/* Enable 5Mib Rom window */
	byte = pci_read_config8(dev, 0x43);
	byte |= 0xc0;
	pci_write_config8(dev, 0x43, byte);

	/* Enable Port 92 fast reset */
	byte = pci_read_config8(dev, 0x41);
	byte |= (1 << 5);
	pci_write_config8(dev, 0x41, byte);

	/* Enable Error reporting */
	/* Set up sync flood detected */
	byte = pci_read_config8(dev, 0x47);
	byte |= (1 << 1);
	pci_write_config8(dev, 0x47, byte);

	/* Set up NMI on errors */
	byte = pci_read_config8(dev, 0x40);
	byte |= (1 << 1); /* clear PW2LPC error */
	byte |= (1 << 6); /* clear LPCERR */
	pci_write_config8(dev, 0x40, byte);
	nmi_option = NMI_OFF;
	get_option(&nmi_option, "nmi");
	if (nmi_option) {			
		byte |= (1 << 7); /* set NMI */
		pci_write_config8(dev, 0x40, byte);
	}
	
	/* Initialize the real time clock */
	rtc_init(0);

	/* Initialize isa dma */
	isa_dma_init();

	/* Initialize the High Precision Event Timers */
	enable_hpet(dev);
}

static void amd8111_lpc_read_resources(struct device * dev)
{
	struct resource *res;

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);

	/* Add an extra subtractive resource for both memory and I/O */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;
	
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;
}

static void amd8111_lpc_enable_resources(struct device * dev)
{
	pci_dev_enable_resources(dev);
	enable_childrens_resources(dev);
}

static void lpci_set_subsystem(struct device * dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, 0x70, 
			   ((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};

struct device_operations amd8111_lpc = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_AMD,
			      .device = PCI_DEVICE_ID_AMD_8111_ISA}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = scan_static_bus,
	.phase3_chip_setup_dev           = amd8111_enable,
	.phase4_read_resources	 = amd8111_lpc_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = amd8111_lpc_enable_resources,
	.phase6_init		 = lpc_init,
	.ops_pci		 = &lops_pci,
};
