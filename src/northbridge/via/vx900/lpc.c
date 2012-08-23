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
#include <drivers/generic/ioapic/chip.h>

#include "vx900.h"

#define INTA 9
#define INTB 10
#define INTC 11
#define INTD 15

/* Restricted to IRQ 10, 11, 14, or 15 */
#define INTSATA 14

typedef struct {
	u8 bus;
	u8 slot;
	u8 irq[4];
} irq_entry;


/* FIXME: IRQ info belongs in chipset or mainboard? */
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

static void vx900_lpc_misc_stuff(device_t dev)
{
	/* GPIO 11,10 to SATALED [1,0] */
	pci_mod_config8(dev, 0xe4, 0 , 1<<0);
}

static void vx900_lpc_dma_setup(device_t dev)
{
	/* These are the steps recommended by VIA in order to get DMA running */

	/* Enable Positive South Module PCI Cycle Decoding */
	/* FIXME: Setting this seems to hang our system */
	//pci_mod_config8(dev, 0x58, 0, 1<<4);
	/* Positive decoding for ROM + APIC + On-board IO ports */
	pci_mod_config8(dev, 0x6c, 0, (1<<2) | (1<<3) | (1<<7));
	/* Enable DMA channels. BIOS guide recommends DMA channel 2 off */
	pci_write_config8(dev, 0x53, 0xfb);
	/* Disable PCI/DMA Memory Cycles Output to PCI Bus */
	pci_mod_config8(dev, 0x5b, (1<<5), 0);
	/* DMA bandwidth control - Improved bandwidth */
	pci_write_config8(dev, 0x53, 0xff);
	/* ISA Positive Decoding control */
	pci_write_config8(dev, 0x6d, 0xdf);
	pci_write_config8(dev, 0x6e, 0x98);
	pci_write_config8(dev, 0x6f, 0x30);
}

/**
 *\brief VX900: Set up the south module IOAPIC (for the ISA/LPC bus)
 *
 * Enable the IOAPIC in the south module, and properly set it up.
 * \n
 * This is the hardware specific initialization for the IOAPIC, and complements
 * the setup done by the generic IOAPIC driver. In order for the IOAPIC to work
 * properly, it _must_ be declared in devicetree.cb .
 * \n
 * We are assuming this is called before the drivers/generic/ioapic code,
 * which should be the case if devicetree.cb is set up properly.
 */
static void vx900_lpc_ioapic_setup(device_t dev)
{
	/* Find the IOAPIC, and make sure it's set up correctly in devicetree.cb
	 * If it's not, then the generic ioapic driver will not set it up
	 * correctly, and the MP table will not be correctly generated */
	device_t ioapic;
	for(ioapic = dev->next; ioapic; ioapic = ioapic->next)
	{
		if(ioapic->path.type == DEVICE_PATH_IOAPIC)
			break;
	}

	/* You did put an IOAPIC in devicetree.cb, didn't you? */
	if(ioapic == 0) {
		/* We don't have enough info to set up the IOAPIC */
		printk(BIOS_ERR, "ERROR: South module IOAPIC not found. "
				 "Check your devicetree.cb\n");
		return;
	}

	/* Found an IOAPIC, now we need to make sure it's the right one */
	ioapic_config_t config = (ioapic_config_t)ioapic->chip_info;
	if(!config->have_isa_interrupts) {
		/* Umh, is this the right IOAPIC ? */
		printk(BIOS_ERR, "ERROR: South module IOAPIC not carrying ISA "
				 "interrupts. Check your devicetree.cb\n");
		printk(BIOS_ERR, "Will not initialize this IOAPIC.\n");
		return;
	}

	/* The base address of this IOAPIC _must_ be at 0xfec00000.
	 * Don't move this value to a #define, as people might think it's
	 * configurable. It is not. */
	const u32 base = config->base;
	if(base != 0xfec00000) {
		printk(BIOS_ERR, "ERROR: South module IOAPIC base should be at "
				 "0xfec00000\n but we found it at 0x%.8x\n",
				 base);
		return;
	}

	print_debug("VX900 LPC: Setting up the south module IOAPIC.\n");
	/* Enable IOAPIC
	 * So much work for one line of code. Talk about bloat :)
	 * The 8259 PIC should still work even if the IOAPIC is enabled, so
	 * there's no crime in enabling the IOAPIC here. */
	pci_mod_config8(dev, 0x58, 0, 1<<6);
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

	/* Route INTA -> INTD to the proper interrupts */
	pci_write_config8(dev, 0x55, INTA<<4);
	pci_write_config8(dev, 0x56, INTB | (INTC<<4) );
	pci_write_config8(dev, 0x57, INTD<<4);
	/* The SATA controller interrupt is more special */
	pci_write_config8(dev, 0x4c, (INTSATA - 14) | (1<<6) );

	/* Get the IRQs up and running. SeaBIOS/linux needs these to boot */
	setup_i8259();
	assign_irqs_from_table();

	vx900_lpc_dma_setup(dev);

	/* The IOAPIC is special, and we treat it separately */
	vx900_lpc_ioapic_setup(dev);
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
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = vx900_lpc_init,
	.scan_bus = scan_static_bus,
};

static const struct pci_driver lpc_driver __pci_driver = {
	.ops = &vx900_lpc_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_VX900_LPC,
};