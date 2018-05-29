/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
#include <pc80/i8259.h>
#include <pc80/keyboard.h>
#include <pc80/isa-dma.h>
#include <cpu/x86/lapic.h>
#include <arch/ioapic.h>
#include <stdlib.h>

#define ACPI_IO_BASE	0x400

static const unsigned char pci_irqs[4] = { 11, 11, 10, 10 };

static const unsigned char usb_pins[4] = { 'A', 'B', 'C', 'D' };
static const unsigned char vga_pins[4] = { 'A', 'B', 'C', 'D' };
static const unsigned char slot_pins[4] = { 'B', 'C', 'D', 'A' };
static const unsigned char ac97_pins[4] = { 'B', 'C', 'D', 'A' };

static unsigned char *pin_to_irq(const unsigned char *pin)
{
	static unsigned char irqs[4];
	int i;
	for (i = 0; i < 4; i++)
		irqs[i] = pci_irqs[pin[i] - 'A'];

	return irqs;
}

static void pci_routing_fixup(struct device *dev)
{
	printk(BIOS_DEBUG, "%s: device is %p\n", __FUNCTION__, dev);

	/* set up PCI IRQ routing */
	pci_write_config8(dev, 0x55, pci_irqs[0] << 4);
	pci_write_config8(dev, 0x56, pci_irqs[1] | (pci_irqs[2] << 4));
	pci_write_config8(dev, 0x57, pci_irqs[3] << 4);

	/* Assigning IRQs */
	printk(BIOS_DEBUG, "Setting up USB interrupts.\n");
	pci_assign_irqs(0, 0x10, pin_to_irq(usb_pins));

	printk(BIOS_DEBUG, "Setting up VGA interrupts.\n");
	pci_assign_irqs(1, 0x00, pin_to_irq(vga_pins));

	printk(BIOS_DEBUG, "Setting up PCI slot interrupts.\n");
	pci_assign_irqs(2, 0x04, pin_to_irq(slot_pins));
	// more?

	printk(BIOS_DEBUG, "Setting up AC97 interrupts.\n");
	pci_assign_irqs(0x80, 0x1, pin_to_irq(ac97_pins));
}

/*
 * Set up the power management capabilities directly into ACPI mode.  This
 * avoids having to handle any System Management Interrupts (SMI's) which I
 * can't figure out how to do !!!!
 */

static void setup_pm(struct device *dev)
{
	/* Debounce LID and PWRBTN# Inputs for 16ms. */
	pci_write_config8(dev, 0x80, 0x20);

	/* Set ACPI base address to IO ACPI_IO_BASE */
	pci_write_config16(dev, 0x88, ACPI_IO_BASE | 1);

	/* set ACPI irq to 9 */
	pci_write_config8(dev, 0x82, 0x49);

	/* Primary interupt channel, define wake events 0 = IRQ0 15 = IRQ15 1 = en. */
	pci_write_config16(dev, 0x84, 0x609a);

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
	/* GP3 Timer Counter */

	/* Multi Function Select 1 */
	pci_write_config8(dev, 0xe4, 0x00);

	/* Multi Function Select 2 */
	pci_write_config8(dev, 0xe5, 0x41);	//??

	/* Enable ACPI access (and setup like award) */
	pci_write_config8(dev, 0x81, 0x84);

	/* Clear status events. */
	outw(0xffff, ACPI_IO_BASE + 0x00);
	outw(0xffff, ACPI_IO_BASE + 0x20);
	outw(0xffff, ACPI_IO_BASE + 0x28);
	outl(0xffffffff, ACPI_IO_BASE + 0x30);

	/* Disable SCI on GPIO. */
	outw(0x0, ACPI_IO_BASE + 0x22);

	/* Disable SMI on GPIO. */
	outw(0x0, ACPI_IO_BASE + 0x24);

	/* Disable all global enable SMIs. */
	outw(0x0, ACPI_IO_BASE + 0x2a);

	/* All SMI off, both IDE buses ON, PSON rising edge. */
	outw(0x0, ACPI_IO_BASE + 0x2c);

	/* Primary activity SMI disable. */
	outl(0x0, ACPI_IO_BASE + 0x34);

	/* GP timer reload on none. */
	outl(0x0, ACPI_IO_BASE + 0x38);

	/* Disable extended IO traps. */
	outb(0x0, ACPI_IO_BASE + 0x42);

	/* SCI is generated for RTC/pwrBtn/slpBtn. */
	outw(0x0001, ACPI_IO_BASE + 0x04);

	/* Allow SLP# signal to assert LDTSTOP_L.
	 * Will work for C3 and for FID/VID change.
	 */
	outb(0x1, ACPI_IO_BASE + 0x11);
}

static void cx700_set_lpc_registers(struct device *dev)
{
	unsigned char enables;

	printk(BIOS_DEBUG, "VIA CX700 LPC bridge init\n");

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

	/* Enable SM dynamic clock gating */
	pci_write_config8(dev, 0x5b, 0x01);

	/* Set Read Pass Write Control Enable */
	pci_write_config8(dev, 0x48, 0x0c);

	/* Set SM Misc Control: Enable Internal APIC . */
	enables = pci_read_config8(dev, 0x58);
	enables |= 1 << 6;
	pci_write_config8(dev, 0x58, enables);
	enables = pci_read_config8(dev, 0x4d);
	enables |= 1 << 3;
	pci_write_config8(dev, 0x4d, enables);

	/* Set bit 3 of 0x4f to match award (use INIT# as CPU reset) */
	enables = pci_read_config8(dev, 0x4f);
	enables |= 0x08;
	pci_write_config8(dev, 0x4f, enables);

	/* enable KBC configuration */
	pci_write_config8(dev, 0x51, 0x1f);

	/* enable serial irq */
	pci_write_config8(dev, 0x52, 0x9);

	/* dma */
	pci_write_config8(dev, 0x53, 0x00);

	// Power management setup
	setup_pm(dev);

	/* set up isa bus -- i/o recovery time, ROM write enable, extend-ale */
	pci_write_config8(dev, 0x40, 0x54);

	/* Enable HPET timer */
	pci_write_config32(dev, 0x68, (1 << 31) | (CONFIG_HPET_ADDRESS >> 8));

}

static void cx700_read_resources(struct device *dev)
{
	struct resource *res;

	/* Make sure we call our childrens set/enable functions - these
	 * are not called unless this device has a resource to set.
	 */

	pci_dev_read_resources(dev);

	res = new_resource(dev, 1);
	res->base = 0x0UL;
	res->size = 0x400UL;
	res->limit = 0xffffUL;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void cx700_set_resources(struct device *dev)
{
	struct resource *resource;
	resource = find_resource(dev, 1);
	resource->flags |= IORESOURCE_STORED;
	pci_dev_set_resources(dev);
}

static void cx700_enable_resources(struct device *dev)
{
	/* Enable SuperIO decoding */
	pci_dev_enable_resources(dev);
}

static void cx700_lpc_init(struct device *dev)
{
	cx700_set_lpc_registers(dev);

#if IS_ENABLED(CONFIG_IOAPIC)
#define IO_APIC_ID 2
	setup_ioapic(VIO_APIC_VADDR, IO_APIC_ID);
#endif

	/* Initialize interrupts */
	pci_routing_fixup(dev);
	/* make sure interupt controller is configured before keyboard init */
	setup_i8259();

	/* Start the Real Time Clock */
	cmos_init(0);

	/* Initialize isa dma */
	isa_dma_init();

	/* Initialize keyboard controller */
	pc_keyboard_init(NO_AUX_DEVICE);
}

static struct device_operations cx700_lpc_ops = {
	.read_resources = cx700_read_resources,
	.set_resources = cx700_set_resources,
	.enable_resources = cx700_enable_resources,
	.init = cx700_lpc_init,
	.scan_bus = scan_lpc_bus,
};

static const struct pci_driver lpc_driver __pci_driver = {
	.ops    = &cx700_lpc_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = 0x8324,
};
