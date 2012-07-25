/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/i8259.h>
#include <pc80/mc146818rtc.h>

#include "vx900.h"

#define INTA 7
#define INTB 6
#define INTC 15
#define INTD 5

/* Restricted to IRQ 10, 11, 14, or 15 */
#define INTSATA 14

typedef struct {
	u8 bus;
	u8 slot;
	u8 irq[4];
} irq_entry;

const static irq_entry irq_table[] = {
	{0, 0x1, {INTA, INTB, INTC, INTD} },             /* VGA */
	{0, 0xf, {INTSATA, INTSATA, INTSATA, INTSATA} }, /* SATA controller */
	{0xff} /* End list */
};

static void assign_irqs_from_table(void)
{
	const irq_entry * irqs = irq_table;
	while(irqs->bus != 0xff) {
		pci_assign_irqs(irqs->bus, irqs->slot, irqs->irq);
		irqs++;
	}
}

static void vx900_lpc_read_resources(device_t dev)
{
	pci_dev_read_resources(dev);
	/* In case we need to do some more stuff */
}

static void vx900_lpc_set_resources(device_t dev)
{
	/* In case we need to do some more stuff */

	pci_dev_set_resources(dev);
}

static void vx900_lpc_misc_stuff(device_t dev)
{
	/* GPIO 11,10 to SATALED [1,0] */
	pci_mod_config8(dev, 0xe4, 0 , 1<<0);
}

static void vx900_lpc_interrupt_stuff(device_t dev)
{
	/* Enable setting trigger mode through 0x4d0, and 0x4d1 ports
	 * And enable I/O recovery time */
	pci_mod_config8(dev, 0x40, 0, (1<<2)|(1<<6));
	/* Set serial IRQ frame width to 6 PCI cycles (recommended by VIA)
	 * And enable serial IRQ */
	pci_mod_config8(dev, 0x52, 3<<0, (1<<3)|(1<<0) );

	/* Disable IRQ12 storm FIXME: bad comment */
	pci_mod_config8(dev, 0x51, (1<<2), 0);

	/* Enable APIC */
	pci_mod_config8(dev, 0x58, 0, 1<<6);

	/* Route INTA -> INTD to the proper interrupts */
	pci_write_config8(dev, 0x55, INTA<<4);
	pci_write_config8(dev, 0x56, INTB | (INTC<<4) );
	pci_write_config8(dev, 0x57, INTD<<4);
	/* The SATA controller interrupt is more special */
	pci_write_config8(dev, 0x4c, (INTSATA - 14) | (1<<6) );

	/* Get the IRQs up and running. SeaBIOS might need these to boot */
	setup_i8259();
	assign_irqs_from_table();

	/* DMA Enable */
	//pci_write_config8(dev, 0x53, 0xff);
}

static void dump_pci_device(device_t dev)
{
	int i;
	for (i = 0; i <= 255; i++) {
		unsigned char val;
		if ((i & 0x0f) == 0) {
			print_debug_hex8(i);
			print_debug_char(':');
		}
		val = pci_read_config8(dev, i);
		if((i & 7) == 0) print_debug(" |");
		print_debug_char(' ');
		print_debug_hex8(val);
		if ((i & 0x0f) == 0x0f) {
			print_debug("\n");
		}
	}
}

static void vx900_lpc_init(device_t dev)
{
	vx900_lpc_interrupt_stuff(dev);
	vx900_lpc_misc_stuff(dev);
	dump_pci_device(dev);
}

static struct device_operations vx900_lpc_ops = {
	.read_resources = vx900_lpc_read_resources,
	.set_resources = vx900_lpc_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = vx900_lpc_init,
	.scan_bus = scan_static_bus,
};

static const struct pci_driver lpc_driver __pci_driver = {
	.ops = &vx900_lpc_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_VX900_LPC,
};