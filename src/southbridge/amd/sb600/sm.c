/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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
#include "sb600.h"
#include "smbus.c"

#define NMI_OFF 0

#define MAINBOARD_POWER_OFF 0
#define MAINBOARD_POWER_ON 1

#ifndef CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif

/*
* SB600 enables all USB controllers by default in SMBUS Control.
* SB600 enables SATA by default in SMBUS Control.
*/
static void sm_init(struct device *dev)
{
	u8 byte;
	u8 byte_old;
	u32 dword;
	u32 ioapic_base;
	u32 on;
	u32 nmi_option;

	printk(BIOS_INFO, "sm_init().\n");

	ioapic_base = pci_read_config32(dev, 0x74) & (0xffffffe0);	/* some like mem resource, but does not have  enable bit */
	/* Don't rename APIC ID */
	clear_ioapic((void *)ioapic_base);

	dword = pci_read_config8(dev, 0x62);
	dword |= 1 << 2;
	pci_write_config8(dev, 0x62, dword);

	dword = pci_read_config32(dev, 0x78);
	dword |= 1 << 9;
	pci_write_config32(dev, 0x78, dword);	/* enable 0xCD6 0xCD7 */

	/* bit 10: MultiMediaTimerIrqEn */
	dword = pci_read_config8(dev, 0x64);
	dword |= 1 << 10;
	pci_write_config8(dev, 0x64, dword);
	/* enable serial irq */
	byte = pci_read_config8(dev, 0x69);
	byte |= 1 << 7;		/* enable serial irq function */
	byte &= ~(0xF << 2);
	byte |= 4 << 2;		/* set NumSerIrqBits=4 */
	pci_write_config8(dev, 0x69, byte);

	byte = pm_ioread(0x61);
	byte |= 1 << 1;		/* Set to enable NB/SB handshake during IOAPIC interrupt for AMD K8/K7 */
	pm_iowrite(0x61, byte);

	/* disable SMI */
	byte = pm_ioread(0x53);
	byte |= 1 << 3;
	pm_iowrite(0x53, byte);

	/* power after power fail */
	on = CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL;
	get_option(&on, "power_on_after_fail");
	byte = pm_ioread(0x74);
	byte &= ~0x03;
	if (on) {
		byte |= 1 << 0;
	}
	byte |= 1 << 2;
	pm_iowrite(0x74, byte);
	printk(BIOS_INFO, "set power %s after power fail\n", on ? "on" : "off");

	/* sb600 rpr:2.3.3: */
	byte = pm_ioread(0x9A);
	byte |= 1 << 5 | 1 << 4 | 1 << 2;
	pm_iowrite(0x9A, byte);

	byte = pm_ioread(0x8F);
	byte |= 1 << 5;
	byte &= ~(1 << 4);
	pm_iowrite(0x8F, byte);

	pm_iowrite(0x8B, 0x01);
	pm_iowrite(0x8A, 0x90);
	pm_iowrite(0x88, 0x10);	/* A21 */

	byte = pm_ioread(0x7C);
	byte |= 1 << 0;
	pm_iowrite(0x7C, byte);

	byte = pm_ioread(0x68);
	byte &= ~(1 << 1);
	/* 2.6 */
	byte |= 1 << 2;
	pm_iowrite(0x68, byte);

	/* 2.6 */
	byte = pm_ioread(0x65);
	byte &= ~(1 << 7);
	pm_iowrite(0x65, byte);

	/* 2.3.4 */
	byte = pm_ioread(0x52);
	byte &= ~0x2F;
	byte |= 0x8;
	pm_iowrite(0x52, byte);

	byte = pm_ioread(0x8D);
	byte &= ~(1 << 6);
	pm_iowrite(0x8D, byte);

	byte = pm_ioread(0x61);
	byte &= ~(1 << 2);
	pm_iowrite(0x61, byte);

	byte = pm_ioread(0x42);
	byte &= ~(1 << 2);
	pm_iowrite(0x42, byte);

	/* Set up NMI on errors */
	byte = inb(0x70);	/* RTC70 */
	byte_old = byte;
	nmi_option = NMI_OFF;
	get_option(&nmi_option, "nmi");
	if (nmi_option) {
		byte &= ~(1 << 7);	/* set NMI */
		printk(BIOS_INFO, "++++++++++set NMI+++++\n");
	} else {
		byte |= (1 << 7);	/* Can not mask NMI from PCI-E and NMI_NOW */
		printk(BIOS_INFO, "++++++++++no set NMI+++++\n");
	}
	byte &= ~(1 << 7);
	if (byte != byte_old) {
		outb(byte, 0x70);
	}

	/* 2.10 IO Trap Settings */
	abcfg_reg(0x10090, 1 << 16, 1 << 16);

	/* ab index */
	pci_write_config32(dev, 0xF0, AB_INDX);
	/* Initialize the real time clock */
	cmos_init(0);

	/*3.4 Enabling IDE/PCIB Prefetch for Performance Enhancement */
	abcfg_reg(0x10060, 9 << 17, 9 << 17);
	abcfg_reg(0x10064, 9 << 17, 9 << 17);

	/* 3.5 Enabling OHCI Prefetch for Performance Enhancement */
	abcfg_reg(0x80, 1 << 0, 1<< 0);

	/* 3.6 B-Link Client's Credit Variable Settings for the Downstream Arbitration Equation */
	/* 3.7 Enabling Additional Address Bits Checking in Downstream */
	abcfg_reg(0x9c, 3 << 0, 3 << 0);

	/* 3.8 Set B-Link Prefetch Mode */
	abcfg_reg(0x80, 3 << 17, 3 << 17);

	/* 3.9 Enabling Detection of Upstream Interrupts */
	abcfg_reg(0x94, 1 << 20,1 << 20);

	/* 3.10: Enabling Downstream Posted Transactions to Pass Non-Posted
	 *  Transactions for the K8 Platform (for All Revisions) */
	abcfg_reg(0x10090, 1 << 8, 1 << 8);

	/* 3.11:Programming Cycle Delay for AB and BIF Clock Gating */
	/* 3.12: Enabling AB and BIF Clock Gating */
	abcfg_reg(0x10054, 0xFFFF0000, 0x1040000);
	abcfg_reg(0x54, 0xFF << 16, 4 << 16);
	printk(BIOS_INFO, "3.11, ABCFG:0x54\n");
	abcfg_reg(0x54, 1 << 24, 1 << 24);
	printk(BIOS_INFO, "3.12, ABCFG:0x54\n");
	abcfg_reg(0x98, 0x0000FF00, 0x00004700);

	/* 3.13:Enabling AB Int_Arbiter Enhancement (for All Revisions) */
	abcfg_reg(0x10054, 0x0000FFFF, 0x07FF);

	/* 3.14:Enabling L1 on A-link Express */
	axcfg_reg(0x68, 0x00000003, 0x2);
	axindxp_reg(0xa0, 0x0000F000, 0x6000);

	abcfg_reg(0x10098, 0xFFFFFFFF, 0x4000);
	abcfg_reg(0x04, 0xFFFFFFFF, 0x6);
	printk(BIOS_INFO, "sm_init() end\n");

	/* Enable NbSb virtual channel */
	axcfg_reg(0x114, 0x3f << 1, 0 << 1);
	axcfg_reg(0x120, 0x7f << 1, 0x7f << 1);
	axcfg_reg(0x120, 7 << 24, 1 << 24);
	axcfg_reg(0x120, 1 << 31, 1 << 31);
	abcfg_reg(0x50, 1 << 3, 1 << 3);
}

static int lsmbus_recv_byte(struct device *dev)
{
	u32 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x10);

	return do_smbus_recv_byte(res->base, device);
}

static int lsmbus_send_byte(struct device *dev, u8 val)
{
	u32 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x10);

	return do_smbus_send_byte(res->base, device, val);
}

static int lsmbus_read_byte(struct device *dev, u8 address)
{
	u32 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x10);

	return do_smbus_read_byte(res->base, device, address);
}

static int lsmbus_write_byte(struct device *dev, u8 address, u8 val)
{
	u32 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x10);

	return do_smbus_write_byte(res->base, device, address, val);
}
static struct smbus_bus_operations lops_smbus_bus = {
	.recv_byte = lsmbus_recv_byte,
	.send_byte = lsmbus_send_byte,
	.read_byte = lsmbus_read_byte,
	.write_byte = lsmbus_write_byte,
};

static void sb600_sm_read_resources(struct device *dev)
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
	res->limit = 0xFFFFFFFFUL;	/* res->base + res->size -1; */
	res->align = 8;
	res->gran = 8;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED;

	res = new_resource(dev, 0x14); /* hpet */
	res->base  = HPET_BASE_ADDRESS;	/* reset hpet to widely accepted address */
	res->size = 0x400;
	res->limit = 0xFFFFFFFFUL;	/* res->base + res->size -1; */
	res->align = 8;
	res->gran = 8;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED;
	/* dev->command |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER; */

	/* smbus */
	res = new_resource(dev, 0x10);
	res->base  = 0xB00;
	res->size = 0x10;
	res->limit = 0xFFFFUL;	/* res->base + res->size -1; */
	res->align = 8;
	res->gran = 8;
	res->flags = IORESOURCE_IO | IORESOURCE_FIXED;

	compact_resources(dev);

}

static void sb600_sm_set_resources(struct device *dev)
{
	struct resource *res;
	u8 byte;

	pci_dev_set_resources(dev);

	/* rpr2.14: Make HPET MMIO decoding controlled by the memory enable bit in command register of LPC ISA bridge */
	byte = pm_ioread(0x52);
	byte |= 1 << 6;
	pm_iowrite(0x52, byte);

	res = find_resource(dev, 0x74);
	pci_write_config32(dev, 0x74, res->base | 1 << 3);

	res = find_resource(dev, 0x14);
	pci_write_config32(dev, 0x14, res->base);

	res = find_resource(dev, 0x10);
	pci_write_config32(dev, 0x10, res->base | 1);
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations smbus_ops = {
	.read_resources = sb600_sm_read_resources,
	.set_resources = sb600_sm_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = sm_init,
	.scan_bus = scan_smbus,
	/* .enable           = sb600_enable, */
	.ops_pci = &lops_pci,
	.ops_smbus_bus = &lops_smbus_bus,
};

static const struct pci_driver smbus_driver __pci_driver = {
	.ops = &smbus_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB600_SM,
};
