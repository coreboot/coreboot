/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/smbus.h>
#include <pc80/mc146818rtc.h>
#include <arch/io.h>
#include <cpu/x86/lapic.h>
#include <arch/ioapic.h>
#include <stdlib.h>
#include "sb800.h"
#include "smbus.c"

#define NMI_OFF 0

#define MAINBOARD_POWER_OFF 0
#define MAINBOARD_POWER_ON 1

#ifndef CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif

#define BIT0	(1 << 0)
#define BIT1	(1 << 1)
#define BIT2	(1 << 2)
#define BIT3	(1 << 3)
#define BIT4	(1 << 4)
#define BIT5	(1 << 5)
#define BIT6	(1 << 6)
#define BIT7	(1 << 7)

#define BIT8	(1 << 8 )
#define BIT9	(1 << 9 )
#define BIT10	(1 << 10)
#define BIT11	(1 << 11)
#define BIT12	(1 << 12)
#define BIT13	(1 << 13)
#define BIT14	(1 << 14)
#define BIT15	(1 << 15)

#define BIT16	(1 << 16)
#define BIT17	(1 << 17)
#define BIT18	(1 << 18)
#define BIT19	(1 << 19)
#define BIT20	(1 << 20)
#define BIT21	(1 << 21)
#define BIT22	(1 << 22)
#define BIT23	(1 << 23)
#define BIT24	(1 << 24)
#define BIT25	(1 << 25)
#define BIT26	(1 << 26)
#define BIT27	(1 << 27)
#define BIT28	(1 << 28)
#define BIT29	(1 << 29)
#define BIT30	(1 << 30)
#define BIT31	(1 << 31)

/*
* SB800 enables all USB controllers by default in SMBUS Control.
* SB800 enables SATA by default in SMBUS Control.
*/

static void sm_init(struct device *dev)
{
	u8 byte;

	printk(BIOS_INFO, "sm_init().\n");

	/* Don't rename APIC ID */
	/* TODO: We should call setup_ioapic() here. But kernel hangs if CPU is K8.
	 * We need to check out why and change back. */
	clear_ioapic(VIO_APIC_VADDR);
	//setup_ioapic(IO_APIC_ADDR, 0);

	/* enable serial irq */
	byte = pm_ioread(0x54);
	byte |= 1 << 7;		/* enable serial irq function */
	byte &= ~(0xF << 2);
	byte |= 4 << 2;		/* set NumSerIrqBits=4 */
	pm_iowrite(0x54, byte);

	pm_iowrite(0x00, 0x0E);
	pm_iowrite(0x0B, 0x02);
	/* 2.11 IO Trap Settings */
	abcfg_reg(0x10090, 1 << 16, 1 << 16);

	/* 4.1 ab index */
	//pci_write_config32(dev, 0xF0, AB_INDX);
	pm_iowrite(0xE0, AB_INDX & 0xFF);
	pm_iowrite(0xE1, (AB_INDX >> 8) & 0xFF);
	pm_iowrite(0xE2, (AB_INDX >> 16) & 0xFF);
	pm_iowrite(0xE3, (AB_INDX >> 24) & 0xFF);
	/* Initialize the real time clock */
	cmos_init(0);

	byte = pm_ioread(0x8);
	byte |= 1 << 2 | 1 << 4;
	pm_iowrite(0x08, byte);
	byte = pm_ioread(0x9);
	byte |= 1 << 0;
	pm_iowrite(0x09, byte);

	abcfg_reg(0x10060, (BIT31), BIT31);
	abcfg_reg(0x1009C, (BIT4 + BIT5), BIT4 + BIT5);
	abcfg_reg(0x9C,    (BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7), BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7);
	abcfg_reg(0x90,    (BIT21 + BIT22 + BIT23), BIT21 + BIT22 + BIT23);
	abcfg_reg(0xF0,    (BIT6 + BIT5), BIT6 + BIT5);
	abcfg_reg(0x10090, (BIT9 + BIT10 + BIT11 + BIT12), BIT9 + BIT10 + BIT11 + BIT12);
	abcfg_reg(0x58,    (BIT10), BIT10);
	abcfg_reg(0xF0,    (BIT3 + BIT4), BIT3 + BIT4);
	abcfg_reg(0x54,    (BIT1), BIT1);
	//
	axindxc_reg(0x02, BIT9, BIT9);
	axindxc_reg(0x10, BIT9, BIT9);

	/* 4.2 Enabling Upstream DMA Access */
	axcfg_reg(0x04, 1 << 2, 1 << 2);
	/* 4.3 Enabling PCIB Prefetch Settings */
	abcfg_reg(0x10060, 1 << 20, 1 << 20);
	abcfg_reg(0x10064, 1 << 20, 1 << 20);

	/* 4.4 Enabling OHCI Prefetch for Performance Enhancement, A12 */
	abcfg_reg(0x80, 1 << 0, 1<< 0);

	/* 4.5 B-Link Client's Credit Variable Settings for the Downstream Arbitration Equation */
	/* 4.6 Enabling Additional Address Bits Checking in Downstream */
	abcfg_reg(0x9c, 1 << 0, 1 << 0);
	//abcfg_reg(0x9c, 3 << 0, 3 << 0); //A11

	/* 4.7 Set B-Link Prefetch Mode */
	abcfg_reg(0x80, 3 << 17, 3 << 17);

	// RPR Enabled SMI ordering enhancement. ABCFG 0x90[21]
	// RPR USB Delay A-Link Express L1 State. ABCFG 0x90[17]
	abcfg_reg(0x90, 1 << 17 | 1 << 21, 1 << 17 | 1 << 21);
	/* 4.8 Enabling Detection of Upstream Interrupts */
	abcfg_reg(0x94, 1 << 20 | 0x7FFFF, 1 << 20 | 0x00FEE);

	/* 4.9: Enabling Downstream Posted Transactions to Pass Non-Posted
	 *  Transactions for the K8 Platform (for All Revisions) */
	abcfg_reg(0x10090, 1 << 8, 1 << 8);

	/* 4.10:Programming Cycle Delay for AB and BIF Clock Gating */
	/* 4.11:Enabling AB Int_Arbiter Enhancement (for All Revisions) */
	abcfg_reg(0x10054, 0xFFFF0000, 0x01040000);
	abcfg_reg(0x54, 0xFF << 16, 4 << 16);
	abcfg_reg(0x54, 1 << 24, 0 << 24);
	abcfg_reg(0x54, 1 << 26, 1 << 26);
	abcfg_reg(0x98, 0xFFFFFF00, 0x00004700);

	/* 4.12: Enabling AB and BIF Clock Gating */
	abcfg_reg(0x10054, 0x0000FFFF, 0x07FF);

	/* 4.13:Enabling Requester ID for upstream traffic. */
	abcfg_reg(0x98, 3 << 16, 3 << 16);

	abcfg_reg(0x50, 1 << 2, 0 << 2);

	/* 5.2 Enabling GPP Port A/B/C/D */
	//abcfg_reg(0xC0, 0xF << 4, 0xF << 4);

	/* Enable SCI as irq9. */
	outb(0x10, 0xC00);
	outb(0x9, 0xC01);
	/* Enabled IRQ input */
	outb(0x9, 0xC00);
	outb(0xF7, 0xC01);

	abcfg_reg(0x90, 0xFFFFFFFF, 0x00F80040);
	abcfg_reg(0xA0, 0xFFFFFFFF, 0x00000000);
	abcfg_reg(0xA4, 0xFFFFFFFF, 0x00000000);
	abcfg_reg(0xC0, 0xFFFFFFFF, 0x0000F014);
	abcfg_reg(0x98, 0xFFFFFFFF, 0X01034700);
}

static int lsmbus_recv_byte(struct device *dev)
{
	u32 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x90);

	return do_smbus_recv_byte(res->base, device);
}

static int lsmbus_send_byte(struct device *dev, u8 val)
{
	u32 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x90);

	return do_smbus_send_byte(res->base, device, val);
}

static int lsmbus_read_byte(struct device *dev, u8 address)
{
	u32 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x90);

	return do_smbus_read_byte(res->base, device, address);
}

static int lsmbus_write_byte(struct device *dev, u8 address, u8 val)
{
	u32 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x90);

	return do_smbus_write_byte(res->base, device, address, val);
}
static struct smbus_bus_operations lops_smbus_bus = {
	.recv_byte = lsmbus_recv_byte,
	.send_byte = lsmbus_send_byte,
	.read_byte = lsmbus_read_byte,
	.write_byte = lsmbus_write_byte,
};

static void sb800_sm_read_resources(struct device *dev)
{
	struct resource *res;
	u8 byte;

	/* rpr2.14: Hides SM bus controller Bar1 where stores HPET MMIO base address */
	byte = pm_ioread(0x55);
	byte |= 1 << 7;
	pm_iowrite(0x55, byte);

	/* Get the normal pci resources of this device */
	/* pci_dev_read_resources(dev); */

	byte = pm_ioread(0x55);
	byte &= ~(1 << 7);
	pm_iowrite(0x55, byte);

	/* apic */
	res = new_resource(dev, 0x74);
	res->base  = IO_APIC_ADDR;
	res->size = 256 * 0x10;
	res->limit = 0xFEFFFFFUL;	/* res->base + res->size -1; */
	res->align = 8;
	res->gran = 8;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED;

	#if 0			       /* Linux ACPI crashes when it is 1. For late debugging. */
	res = new_resource(dev, 0x14); /* TODO: hpet */
	res->base  = 0xfed00000;	/* reset hpet to widely accepted address */
	res->size = 0x400;
	res->limit = 0xFFFFFFFFUL;	/* res->base + res->size -1; */
	res->align = 8;
	res->gran = 8;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED;
	#endif
	/* dev->command |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER; */

	/* smbus */
	//res = new_resource(dev, 0x90);
	//res->base  = 0xB00;
	//res->size = 0x10;
	//res->limit = 0xFFFFUL;	/* res->base + res->size -1; */
	//res->align = 8;
	//res->gran = 8;
	//res->flags = IORESOURCE_IO | IORESOURCE_FIXED;


	compact_resources(dev);
}

static void sb800_sm_set_resources(struct device *dev)
{
	struct resource *res;
	u8 byte;

	pci_dev_set_resources(dev);


	/* rpr2.14: Make HPET MMIO decoding controlled by the memory enable bit in command register of LPC ISA bridge */
	byte = pm_ioread(0x52);
	byte |= 1 << 6;
	pm_iowrite(0x52, byte);

	res = find_resource(dev, 0x74);

	printk(BIOS_INFO, "sb800_sm_set_resources, res->base=0x%llx\n", res->base);

	//pci_write_config32(dev, 0x74, res->base | 1 << 3);
	pm_iowrite(0x34, res->base | 0x7);
	pm_iowrite(0x35, (res->base >> 8) & 0xFF);
	pm_iowrite(0x36, (res->base >> 16) & 0xFF);
	pm_iowrite(0x37, (res->base >> 24) & 0xFF);
#if 0				/* TODO:hpet */
	res = find_resource(dev, 0x14);
	pci_write_config32(dev, 0x14, res->base);
#endif
	//res = find_resource(dev, 0x90);
	//pci_write_config32(dev, 0x90, res->base | 1);
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};
static struct device_operations smbus_ops = {
	.read_resources = sb800_sm_read_resources,
	.set_resources = sb800_sm_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = sm_init,
	.scan_bus = scan_smbus,
	.ops_pci = &lops_pci,
	.ops_smbus_bus = &lops_smbus_bus,
};
static const struct pci_driver smbus_driver __pci_driver = {
	.ops = &smbus_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_SM,
};
