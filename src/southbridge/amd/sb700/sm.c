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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
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
#include "sb700.h"
#include "smbus.h"

#define NMI_OFF 0

#define MAINBOARD_POWER_OFF 0
#define MAINBOARD_POWER_ON 1

#ifndef CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif

/*
* SB700 enables all USB controllers by default in SMBUS Control.
* SB700 enables SATA by default in SMBUS Control.
*/
static void sm_init(device_t dev)
{
	u8 byte;
	u8 byte_old;
	u8 rev;
	u32 dword;
	u32 ioapic_base;
	u32 on;
	u32 nmi_option;

	printk(BIOS_INFO, "sm_init().\n");

	rev = get_sb700_revision(dev);
	ioapic_base = pci_read_config32(dev, 0x74) & (0xffffffe0);	/* some like mem resource, but does not have  enable bit */
	/* Don't rename APIC ID */
	/* TODO: We should call setup_ioapic() here. But kernel hangs if cpu is K8.
	 * We need to check out why and change back. */
	clear_ioapic(ioapic_base);

	/* 2.10 Interrupt Routing/Filtering */
	dword = pci_read_config8(dev, 0x62);
	dword |= 3;
	pci_write_config8(dev, 0x62, dword);

	/* Delay back to back interrupts to the CPU. */
	dword = pci_read_config16(dev, 0x64);
	dword |= 1 << 13;
	pci_write_config16(dev, 0x64, dword);

	/* rrg:K8 INTR Enable (BIOS should set this bit after PIC initialization) */
	/* rpr 2.1 Enabling Legacy Interrupt */
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

	/* Sx State Settings
	 * Note: These 2 registers need to be set correctly for the S-state
	 * to work properly. Otherwise the system may hang during resume
	 * from the S-state.
	 */
	/*Use 8us clock for delays in the S-state resume timing sequence.*/
	byte = pm_ioread(0x65);
	byte &= ~(1 << 7);
	pm_iowrite(0x65, byte);
	/* Delay the APIC interrupt to the CPU until the system has fully resumed from the S-state. */
	byte = pm_ioread(0x68);
	byte |= 1 << 2;
	pm_iowrite(0x68, byte);

	/* IRQ0From8254 */
	byte = pci_read_config8(dev, 0x41);
	byte &= ~(1 << 7);
	pci_write_config8(dev, 0x41, byte);

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
		byte |= 2;
	}
	byte |= 1 << 2;
	pm_iowrite(0x74, byte);
	printk(BIOS_INFO, "set power %s after power fail\n", on ? "on" : "off");

	byte = pm_ioread(0x68);
	byte &= ~(1 << 1);
	/* 2.7 */
	byte |= 1 << 2;
	pm_iowrite(0x68, byte);

	/* 2.7 */
	byte = pm_ioread(0x65);
	byte &= ~(1 << 7);
	pm_iowrite(0x65, byte);

	/* 2.16 */
	byte = pm_ioread(0x55);
	byte |= 1 << 5;
	pm_iowrite(0x55, byte);

	byte = pm_ioread(0xD7);
	byte |= 1 << 6 | 1 << 1;;
	pm_iowrite(0xD7, byte);

	/* 2.15 */
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

	/*rpr v2.13  2.22 SMBUS PCI Config */
 	byte = pci_read_config8(dev, 0xE1);
	if ((REV_SB700_A11 == rev) || REV_SB700_A12 == rev) {
		byte |= 1 << 0;
	}
	/*Set bit2 to 1, enable Io port 60h read/wrire SMi trapping and
	 *Io port 64h write Smi trapping. conflict with ps2 keyboard
	 */
	//byte |= 1 << 2 | 1 << 3 | 1 << 4;
	byte |= 1 << 3 | 1 << 4;
 	pci_write_config8(dev, 0xE1, byte);

	/* 2.5 Enabling Non-Posted Memory Write */
       	axindxc_reg(0x10, 1 << 9, 1 << 9);

	/* 2.11 IO Trap Settings */
	abcfg_reg(0x10090, 1 << 16, 1 << 16);

	/* ab index */
	pci_write_config32(dev, 0xF0, AB_INDX);
	/* Initialize the real time clock */
	rtc_init(0);

	/* 4.3 Enabling Upstream DMA Access */
	axcfg_reg(0x04, 1 << 2, 1 << 2);
	/* 4.4 Enabling IDE/PCIB Prefetch for Performance Enhancement */
	abcfg_reg(0x10060, 9 << 17, 9 << 17);
	abcfg_reg(0x10064, 9 << 17, 9 << 17);

	/* 4.5 Enabling OHCI Prefetch for Performance Enhancement, A12 */
	abcfg_reg(0x80, 1 << 0, 1<< 0);

	/* 4.6 B-Link Client's Credit Variable Settings for the Downstream Arbitration Equation */
	/* 4.7 Enabling Additional Address Bits Checking in Downstream */
	/* 4.16 IO write and SMI ordering enhancement*/
	abcfg_reg(0x9c, 3 << 0, 3 << 0);
	if (REV_SB700_A12 == rev) {
		abcfg_reg(0x9c, 1 << 8, 1 << 8);
	} else if (rev >= REV_SB700_A14) {
		abcfg_reg(0x9c, 1 << 8, 0 << 8);
	}
	if (REV_SB700_A15 == rev) {
		abcfg_reg(0x90, 1 << 21, 1 << 21);
		abcfg_reg(0x9c, 1 << 5 | 1 << 9 | 1 << 15, 1 << 5 | 1 << 9 | 1 << 15);
	}

	/* 4.8 Set B-Link Prefetch Mode */
	abcfg_reg(0x80, 3 << 17, 3 << 17);

	/* 4.9 Enabling Detection of Upstream Interrupts */
	abcfg_reg(0x94, 1 << 20 | 0x7FFFF, 1 << 20 | 0x00FEE);

	/* 4.10: Enabling Downstream Posted Transactions to Pass Non-Posted
	 *  Transactions for the K8 Platform (for All Revisions) */
	abcfg_reg(0x10090, 1 << 8, 1 << 8);

	/* Set ACPI Software clock Throttling Period to 244 us*/
	byte = pm_ioread(0x68);
	byte &= ~(3 << 6);
	byte |= (2 << 6);	/* 244us */
	pm_iowrite(0x68, byte);

	if (REV_SB700_A15 == rev) {
		u16 word;

		/* rpr v2.13 4.18 Enabling Posted Pass Non-Posted Downstream */
        	axindxc_reg(0x02, 1 << 9, 1 << 9);
		abcfg_reg(0x9C, 0x00007CC0, 0x00007CC0);
		abcfg_reg(0x1009C, 0x00000030, 0x00000030);
		abcfg_reg(0x10090, 0x00001E00, 0x00001E00);

		/* rpr v2.13 4.19 Enabling Posted Pass Non-Posted Upstream */
		abcfg_reg(0x58, 0x0000F800, 0x0000E800);

		/* rpr v2.13 4.20 64 bit Non-Posted Memory Write Support */
        	axindxc_reg(0x02, 1 << 10, 1 << 10);

		/* rpr v2.13 2.38 Unconditional Shutdown */
 		byte = pci_read_config8(dev, 0x43);
		byte &= ~(1 << 3);
 		pci_write_config8(dev, 0x43, byte);

		word = pci_read_config16(dev, 0x38);
		word |= 1 << 12;
 		pci_write_config16(dev, 0x38, word);

		byte |= 1 << 3;
 		pci_write_config8(dev, 0x43, byte);
	}
	//ACPI_DISABLE_TIMER_IRQ_ENHANCEMENT_FOR_8254_TIMER
 	byte = pci_read_config8(dev, 0xAE);
	byte |= 1 << 5;
 	pci_write_config8(dev, 0xAE, byte);

	/* 4.11:Programming Cycle Delay for AB and BIF Clock Gating */
	/* 4.12: Enabling AB and BIF Clock Gating */
	abcfg_reg(0x10054, 0xFFFF0000, 0x1040000);
	abcfg_reg(0x54, 0xFF << 16, 4 << 16);
	abcfg_reg(0x54, 1 << 24, 0 << 24);
	abcfg_reg(0x98, 0x0000FF00, 0x00004700);

	/* 4.13:Enabling AB Int_Arbiter Enhancement (for All Revisions) */
	abcfg_reg(0x10054, 0x0000FFFF, 0x07FF);

	/* 4.14:Enabling Requester ID for upstream traffic. */
	abcfg_reg(0x98, 1 << 16, 1 << 16);

	/* 9.2: Enabling IDE Data Bus DD7 Pull Down Resistor */
	byte = pm2_ioread(0xE5);
	byte |= 1 << 2;
	pm2_iowrite(0xE5, byte);

	/* Enable IDE controller. */
	byte = pm_ioread(0x59);
	byte &= ~(1 << 1);
	pm_iowrite(0x59, byte);

	printk(BIOS_INFO, "sm_init() end\n");

	/* Enable NbSb virtual channel */
	axcfg_reg(0x114, 0x3f << 1, 0 << 1);
	axcfg_reg(0x120, 0x7f << 1, 0x7f << 1);
	axcfg_reg(0x120, 7 << 24, 1 << 24);
	axcfg_reg(0x120, 1 << 31, 1 << 31);
	abcfg_reg(0x50, 1 << 3, 1 << 3);
}

static int lsmbus_recv_byte(device_t dev)
{
	u32 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x90);

	return do_smbus_recv_byte(res->base, device);
}

static int lsmbus_send_byte(device_t dev, u8 val)
{
	u32 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x90);

	return do_smbus_send_byte(res->base, device, val);
}

static int lsmbus_read_byte(device_t dev, u8 address)
{
	u32 device;
	struct resource *res;
	struct bus *pbus;

	device = dev->path.i2c.device;
	pbus = get_pbus_smbus(dev);

	res = find_resource(pbus->dev, 0x90);

	return do_smbus_read_byte(res->base, device, address);
}

static int lsmbus_write_byte(device_t dev, u8 address, u8 val)
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

static void sb700_sm_read_resources(device_t dev)
{
	struct resource *res;

	/* Get the normal pci resources of this device */
	/* pci_dev_read_resources(dev); */

	/* apic */
	res = new_resource(dev, 0x74);
	res->base  = IO_APIC_ADDR;
	res->size = 256 * 0x10;
	res->limit = 0xFFFFFFFFUL;	/* res->base + res->size -1; */
	res->align = 8;
	res->gran = 8;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_RESERVE | IORESOURCE_ASSIGNED;

	/* HPET */
	res = new_resource(dev, 0xB4);	/* TODO: test hpet */
	res->base  = 0xfed00000;	/* reset hpet to widely accepted address */
	res->size = 0x400;
	res->limit = 0xFFFFFFFFUL;	/* res->base + res->size -1; */
	res->align = 8;
	res->gran = 8;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_RESERVE | IORESOURCE_ASSIGNED;

	/* dev->command |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER; */

	/* smbus */
	res = new_resource(dev, 0x90);
	res->base  = 0xB00;
	res->size = 0x10;
	res->limit = 0xFFFFUL;	/* res->base + res->size -1; */
	res->align = 8;
	res->gran = 8;
	res->flags = IORESOURCE_IO | IORESOURCE_FIXED | IORESOURCE_RESERVE | IORESOURCE_ASSIGNED;

	compact_resources(dev);
}

static void sb700_sm_set_resources(struct device *dev)
{
	struct resource *res;
	u8 byte;

	pci_dev_set_resources(dev);
	res = find_resource(dev, 0x74);
	pci_write_config32(dev, 0x74, res->base | 1 << 3);

	/* TODO: test hpet */
#if 0	//rrg-2.0.3 shows BAR1 not used
	/* Make SMBUS BAR1(HPET base at offset 14h) visible */
	byte = pci_read_config8(dev, 0x43);
	byte &= ~(1 << 3);
	pci_write_config8(dev, 0x43, byte);
#endif

	res = find_resource(dev, 0xB4);
	/* Program HPET BAR Address */
	pci_write_config32(dev, 0xB4, res->base);

	/* Enable decoding of HPET MMIO, enable HPET MSI */
	byte = pci_read_config8(dev, 0x43);
	//byte |= (1 << 3); // Make SMBus Bar1 invisible
	//byte |= ((1 << 4) | (1 << 5) | (1 << 6) | (1 << 7));
	byte |= (1 << 4);
	pci_write_config8(dev, 0x43, byte);

	/* Enable HPET irq */
	byte = pci_read_config8(dev, 0x65);
	byte |= (1 << 2);
	pci_write_config8(dev, 0x65, byte);
	/* TODO: End of test hpet */

	res = find_resource(dev, 0x90);
	pci_write_config32(dev, 0x90, res->base | 1);
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations smbus_ops = {
	.read_resources = sb700_sm_read_resources,
	.set_resources = sb700_sm_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = sm_init,
	.scan_bus = scan_static_bus,
	.ops_pci = &lops_pci,
	.ops_smbus_bus = &lops_smbus_bus,
};

static const struct pci_driver smbus_driver __pci_driver = {
	.ops = &smbus_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB700_SM,
};
