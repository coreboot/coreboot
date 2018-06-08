/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
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

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>

#include <pc80/mc146818rtc.h>
#include <pc80/keyboard.h>
#include <pc80/i8259.h>
#include "vx800.h"

static const unsigned char pciIrqs[4] = { 0xa, 0x9, 0xb, 0xa };

static const unsigned char vgaPins[4] = { 'A', 'B', 'C', 'D' };	//only INTA

static const unsigned char slotPins[4] = { 'A', 'A', 'A', 'A' };	//all 4

static const unsigned char usbdevicePins[4] = { 'A', 'B', 'C', 'D' };	//only INTA
static const unsigned char sdioPins[4] = { 'A', 'B', 'C', 'D' };	//only INTA
static const unsigned char sd_ms_ctrl_Pins[4] = { 'B', 'C', 'D', 'A' };	//only INTA
static const unsigned char ce_ata_nf_ctrl_Pins[4] = { 'C', 'C', 'D', 'A' };	//only INTA

static const unsigned char hdacaudioPins[4] = { 'B', 'C', 'D', 'A' };	//only INTA

static unsigned char *pin_to_irq(const unsigned char *pin)
{
	static unsigned char Irqs[4];
	int i;
	for (i = 0; i < 4; i++)
		Irqs[i] = pciIrqs[pin[i] - 'A'];

	return Irqs;
}

static void pci_routing_fixup(struct device *dev)
{
	printk(BIOS_INFO, "%s: dev is %p\n", __FUNCTION__, dev);

	/* set up PCI IRQ routing */
	pci_write_config8(dev, 0x55, pciIrqs[0] << 4);
	pci_write_config8(dev, 0x56, pciIrqs[1] | (pciIrqs[2] << 4));
	pci_write_config8(dev, 0x57, pciIrqs[3] << 4);

	/* VGA */
	printk(BIOS_INFO, "setting vga\n");
	pci_assign_irqs(0, 0x1, pin_to_irq(vgaPins));

	/* PCI slot */
	printk(BIOS_INFO, "setting pci slot\n");
	pci_assign_irqs(0, 0x08, pin_to_irq(slotPins));

	/* PCI slot */
	printk(BIOS_INFO, "setting USB Device Controller\n");
	pci_assign_irqs(0, 0x0b, pin_to_irq(usbdevicePins));

	/* PCI slot */
	printk(BIOS_INFO, "setting SDIO Controller\n");
	pci_assign_irqs(0, 0x0c, pin_to_irq(sdioPins));

	/* PCI slot */
	printk(BIOS_INFO, "setting SD $ MS Controller\n");
	pci_assign_irqs(0, 0x0d, pin_to_irq(sd_ms_ctrl_Pins));

	/* PCI slot */
	printk(BIOS_INFO, "setting CE-ATA NF Controller(Card Boot)\n");
	pci_assign_irqs(0, 0x0e, pin_to_irq(ce_ata_nf_ctrl_Pins));

	/* PCI slot */
	printk(BIOS_INFO, "setting ide\n");

	/* Standard usb components */
	printk(BIOS_INFO, "setting usb1-2\n");

	/* sound hardware */
	printk(BIOS_INFO, "setting hdac audio\n");
	pci_assign_irqs(0, 0x14, pin_to_irq(hdacaudioPins));

	printk(BIOS_SPEW, "%s: DONE\n", __FUNCTION__);
}

static void setup_pm(struct device *dev)
{
	u16 tmp;
	/* Debounce LID and PWRBTN# Inputs for 16ms. */
	pci_write_config8(dev, 0x80, 0x20);

	/* Set ACPI base address to IO VX800_ACPI_IO_BASE */
	pci_write_config16(dev, 0x88, VX800_ACPI_IO_BASE | 1);

	/* set ACPI irq to 9 */
	pci_write_config8(dev, 0x82, 0x49);

	/* Primary interupt channel, define wake events 0 = IRQ0 15 = IRQ15 1 = en. */
	pci_write_config16(dev, 0x84, 0x609a);	// 0x609a??

	/* SMI output level to low, 7.5us throttle clock */
	pci_write_config8(dev, 0x8d, 0x18);

	/* GP Timer Control 1s */
	pci_write_config8(dev, 0x93, 0x88);

	/* Power Well */
	pci_write_config8(dev, 0x94, 0x20);	// 0x20??

	/* 7 = stp to sust delay 1msec
	 * 6 = SUSST# Deasserted Before PWRGD for STD
	 */
	pci_write_config8(dev, 0x95, 0xc0);	// 0xc1??

	/* Disable GP2 & GP3 Timer */
	pci_write_config8(dev, 0x98, 0);

	/* GP2 Timer Counter */
	pci_write_config8(dev, 0x99, 0xfb);

	/* Multi Function Select 1 */
	pci_write_config8(dev, 0xe4, 0x00);
	/* Multi Function Select 2 */
	pci_write_config8(dev, 0xe5, 0x41);	//??

	/* Enable ACPI access (and setup like award) */
	pci_write_config8(dev, 0x81, 0x84);

	/* Clear status events. */
	outw(0xffff, VX800_ACPI_IO_BASE + 0x00);
	outw(0xffff, VX800_ACPI_IO_BASE + 0x20);
	outw(0xffff, VX800_ACPI_IO_BASE + 0x28);
	outl(0xffffffff, VX800_ACPI_IO_BASE + 0x30);

	/* Disable SCI on GPIO. */
	outw(0x0, VX800_ACPI_IO_BASE + 0x22);

	/* Disable SMI on GPIO. */
	outw(0x0, VX800_ACPI_IO_BASE + 0x24);

	/* Disable all global enable SMIs. */
	outw(0x0, VX800_ACPI_IO_BASE + 0x2a);

	/* All SMI off, both IDE buses ON, PSON rising edge. */
	outw(0x0, VX800_ACPI_IO_BASE + 0x2c);

	/* Primary activity SMI disable. */
	outl(0x0, VX800_ACPI_IO_BASE + 0x34);

	/* GP timer reload on none. */
	outl(0x0, VX800_ACPI_IO_BASE + 0x38);

	/* Disable extended IO traps. */
	outb(0x0, VX800_ACPI_IO_BASE + 0x42);

	tmp = inw(VX800_ACPI_IO_BASE + 0x04);
	/* SCI is generated for RTC/pwrBtn/slpBtn. */
	tmp |= 1;
	outw(tmp, VX800_ACPI_IO_BASE + 0x04);

	/* Allow SLP# signal to assert LDTSTOP_L.
	 * Will work for C3 and for FID/VID change.
	 */
	outb(0x1, VX800_ACPI_IO_BASE + 0x11);
}

static void S3_ps2_kb_ms_wakeup(struct device *dev)
{
	u8 enables;
	enables = pci_read_config8(dev, 0x51);
	enables |= 2;
	pci_write_config8(dev, 0x51, enables);

	outb(0xe0, 0x2e);
	outb(0x0b, 0x2f);	//if 09,then only support kb wakeup

	outb(0xe1, 0x2e);	//set any key scan code can wakeup
	outb(0x00, 0x2f);

	outb(0xe9, 0x2e);	//set any mouse scan code can wakeup
	outb(0x00, 0x2f);

	enables &= 0xd;
	pci_write_config8(dev, 0x51, enables);

	outb(inb(VX800_ACPI_IO_BASE + 0x02) | 0x20, VX800_ACPI_IO_BASE + 0x02);	//ACPI golabe enable for sci smi trigger
	outw(inw(VX800_ACPI_IO_BASE + 0x22) | 0x204, VX800_ACPI_IO_BASE + 0x22);	//ACPI SCI on Internal KBC PME and mouse PME
}

static void S3_usb_wakeup(struct device *dev)
{
	outw(inw(VX800_ACPI_IO_BASE + 0x22) | 0x4000, VX800_ACPI_IO_BASE + 0x22);	//SCI on USB PME
}

static void S3_lid_wakeup(struct device *dev)
{
	outw(inw(VX800_ACPI_IO_BASE + 0x22) | 0x800, VX800_ACPI_IO_BASE + 0x22);	//SCI on LID PME
}

/* This looks good enough to work, maybe */
static void vx800_sb_init(struct device *dev)
{
	unsigned char enables;

	// enable the internal I/O decode
	enables = pci_read_config8(dev, 0x6C);
	enables |= 0x80;
	pci_write_config8(dev, 0x6C, enables);

	// Map 4MB of FLASH into the address space

	// Set bit 6 of 0x40, because Award does it (IO recovery time)
	// IMPORTANT FIX - EISA 0x4d0 decoding must be on so that PCI
	// interrupts can be properly marked as level triggered.
	enables = pci_read_config8(dev, 0x40);
	enables |= 0x44;
	pci_write_config8(dev, 0x40, enables);

	/* DMA Line buffer control */
	enables = pci_read_config8(dev, 0x42);
	enables |= 0xf0;
	pci_write_config8(dev, 0x42, enables);

	/* I/O recovery time */
	pci_write_config8(dev, 0x4c, 0x44);

	/* ROM memory cycles go to LPC. */
	pci_write_config8(dev, 0x59, 0x80);

	/* Set 0x5b to 0x01 to match Award */
	enables = pci_read_config8(dev, 0x5b);
	enables |= 0x01;
	pci_write_config8(dev, 0x5b, enables);

	/* Set Read Pass Write Control Enable */
	pci_write_config8(dev, 0x48, 0x0c);

	/* Set 0x58 to 0x42 APIC and RTC. */
	enables = pci_read_config8(dev, 0x58);
	enables |= 0x41;	//
	pci_write_config8(dev, 0x58, enables);

	/* Set bit 3 of 0x4f to match award (use INIT# as CPU reset) */
	enables = pci_read_config8(dev, 0x4f);
	enables |= 0x08;
	pci_write_config8(dev, 0x4f, enables);

	/* enable serial irq */
	pci_write_config8(dev, 0x52, 0x9);

	/* dma */
	pci_write_config8(dev, 0x53, 0x00);

	// Power management setup
	setup_pm(dev);

	/* set up isa bus -- i/o recovery time, ROM write enable, extend-ale */
	pci_write_config8(dev, 0x40, 0x54);

	// Start the rtc
	cmos_init(0);
}

/* total kludge to get lxb to call our childrens set/enable functions - these are
   not called unless this device has a resource to set - so set a dummy one */
static void vx800_read_resources(struct device *dev)
{

	struct resource *resource;
	pci_dev_read_resources(dev);
	resource = new_resource(dev, 1);
	resource->flags |=
	    IORESOURCE_FIXED | IORESOURCE_ASSIGNED | IORESOURCE_IO |
	    IORESOURCE_STORED;
	resource->size = 2;
	resource->base = 0x2e;
}

static void vx800_set_resources(struct device *dev)
{
	struct resource *resource;
	resource = find_resource(dev, 1);
	resource->flags |= IORESOURCE_STORED;
	pci_dev_set_resources(dev);
}

static void southbridge_init(struct device *dev)
{
	printk(BIOS_DEBUG, "vx800 sb init\n");
	vx800_sb_init(dev);
	pci_routing_fixup(dev);

	setup_i8259();		// make sure interupt controller is configured before keyboard init

	/* turn on keyboard and RTC, no need to visit this reg twice */
	pc_keyboard_init(NO_AUX_DEVICE);

	printk(BIOS_DEBUG, "ps2 usb lid, you  set who can wakeup system from s3 sleep\n");
	S3_ps2_kb_ms_wakeup(dev);
	S3_usb_wakeup(dev);
	S3_lid_wakeup(dev);

/*	enable acpi CPU c3 state. (c2 state need not do anything.)
	#1
		fadt->pm2_cnt_blk = 0x22;//to support cpu-c3
		fadt->p_lvl2_lat = 0x50; //this is the coreboot source
		fadt->p_lvl3_lat = 0x320;//
		fadt->pm2_cnt_len = 1;//to support cpu-c3
	#2
		ssdt? ->every CPU has a P_BLK address. set it to 0x10 (so that "Read Processor Level3 register(PMIORx15<7:0>) to enter C3 state"---VIA vx800 P SPEC )
	#3    write 0x17 in to PMIO = VX800_ACPI_IO_BASE + 0x26, following the describtion in the P-spec.
		1  enable SLP# asserts in C3 state  PMIORx26<1> = 1
		2  enable CPUSTP# asserts in C3 state;  PMIORx26<2> = 1
		3  CLKRUN# is always asserted  PMIORx26<3> = 0
		4  Disable PCISTP# When CLKRUN# is asserted
		1: PCISTP# will not assert When CLKRUN# is asserted
		PMIORx26<4> = 1
		5  This bit controls whether the CPU voltage is lowered when in C3/S1 state.
		VRDSLP will be active in either this bit set in C3 or LVL4 register read
		PMIORx26<0> =0
		6  Read Processor Level3 register(PMIORx15<7:0>) to enter C3 state  PMIORx15
	*/
	outb(0x17, VX800_ACPI_IO_BASE + 0x26);

}

static struct device_operations vx800_lpc_ops = {
	.read_resources = vx800_read_resources,
	.set_resources = vx800_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = southbridge_init,
	.scan_bus = scan_lpc_bus,
};

static const struct pci_driver lpc_driver __pci_driver = {
	.ops = &vx800_lpc_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_VX855_LPC,
};
