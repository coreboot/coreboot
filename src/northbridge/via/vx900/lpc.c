/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012-2013 Alexandru Gagniuc <mr.nuke.me@gmail.com>
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
 */

#include <arch/io.h>
#include <arch/pirq_routing.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/i8259.h>
#include <pc80/mc146818rtc.h>
#include <drivers/generic/ioapic/chip.h>

#include "vx900.h"
#include "chip.h"

/**
 * @file vx900/lpc.c
 *
 * STATUS:
 * We do a fair bit of setup, and most of it seems to work fairly well. There
 * are still a few FIXME items here and there, but overall, this code hasn't
 * been touched much from its initial 2012 version to 2013, when it was revived.
 *
 * We do the IOAPIC setup with the assumption that it is declared in the
 * mainboard's devicetree.cb. We cannot use the IOAPIC however. The interrupts
 * do not make it to the CPU. This issue is still under investigation.
 *
 * We also route PIRQs with CONFIG_PIRQ_ROUTE. This is currently the only way to
 * get interrupts working.
 *
 * On the VX900, the keyboard can be connected directly to the chipset
 * (referenced as "internal keyboard" in the documents). As long as that is the
 * case (not connected to the superIO), and we disable the superIO keyboard LDN,
 * it will work, but perhaps this should be more configurable.
 */

static void vx900_lpc_misc_stuff(struct device *dev)
{
	char extint;
	u8 val;
	struct northbridge_via_vx900_config *nb = (void *)dev->chip_info;

	/* GPIO 11,10 to SATALED [1,0] */
	pci_mod_config8(dev, 0xe4, 0, 1 << 0);

	/* Route the external interrupt line */
	extint = nb->ext_int_route_to_pirq;
	if (extint < 'A' || extint > 'H') {
		printk(BIOS_WARNING, "Invalid PIRQ%c for external interrupt\n",
		       extint);
	} else {
		printk(BIOS_INFO, "Routing external interrupt to PIRQ%c\n",
		       extint);
		val = extint - 'A';
		val |= (1 << 3);	/* bit3 enables the external int */
		pci_mod_config8(dev, 0x55, 0xf, val);

	}
}

static void vx900_lpc_dma_setup(struct device *dev)
{
	/* These are the steps recommended by VIA in order to get DMA running */

	/* Enable Positive South Module PCI Cycle Decoding */
	/* FIXME: Setting this seems to hang our system */

	/* Positive decoding for ROM + APIC + On-board IO ports */
	pci_mod_config8(dev, 0x6c, 0, (1 << 2) | (1 << 3) | (1 << 7));
	/* Enable DMA channels. BIOS guide recommends DMA channel 2 off */
	pci_write_config8(dev, 0x53, 0xfb);
	/* Disable PCI/DMA Memory Cycles Output to PCI Bus */
	pci_mod_config8(dev, 0x5b, (1 << 5), 0);
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
static void vx900_lpc_ioapic_setup(struct device *dev)
{
	/* Find the IOAPIC, and make sure it's set up correctly in devicetree.cb
	 * If it's not, then the generic ioapic driver will not set it up
	 * correctly, and the MP table will not be correctly generated */
	struct device *ioapic;
	for (ioapic = dev->next; ioapic; ioapic = ioapic->next) {
		if (ioapic->path.type == DEVICE_PATH_IOAPIC)
			break;
	}

	/* You did put an IOAPIC in devicetree.cb, didn't you? */
	if (ioapic == 0) {
		/* We don't have enough info to set up the IOAPIC */
		printk(BIOS_ERR, "ERROR: South module IOAPIC not found. "
		       "Check your devicetree.cb\n");
		return;
	}

	/* Found an IOAPIC, now we need to make sure it's the right one */
	ioapic_config_t *config = (ioapic_config_t *) ioapic->chip_info;
	if (!config->have_isa_interrupts) {
		/* Umh, is this the right IOAPIC ? */
		printk(BIOS_ERR, "ERROR: South module IOAPIC not carrying ISA "
		       "interrupts. Check your devicetree.cb\n");
		printk(BIOS_ERR, "Will not initialize this IOAPIC.\n");
		return;
	}

	/* The base address of this IOAPIC _must_ be at 0xfec00000.
	 * Don't move this value to a #define, as people might think it's
	 * configurable. It is not. */
	const void *base = config->base;
	if (base != (void *)0xfec00000) {
		printk(BIOS_ERR, "ERROR: South module IOAPIC base should be at "
		       "0xfec00000\n but we found it at %p\n", base);
		return;
	}

	printk(BIOS_DEBUG, "VX900 LPC: Setting up the south module IOAPIC.\n");
	/* Enable IOAPIC
	 * So much work for one line of code. Talk about bloat :)
	 * The 8259 PIC should still work even if the IOAPIC is enabled, so
	 * there's no crime in enabling the IOAPIC here. */
	pci_mod_config8(dev, 0x58, 0, 1 << 6);
}

static void vx900_lpc_interrupt_stuff(struct device *dev)
{
	/* Enable setting trigger mode through 0x4d0, and 0x4d1 ports
	 * And enable I/O recovery time */
	pci_mod_config8(dev, 0x40, 0, (1 << 2) | (1 << 6));
	/* Set serial IRQ frame width to 6 PCI cycles (recommended by VIA)
	 * And enable serial IRQ */
	pci_mod_config8(dev, 0x52, 3 << 0, (1 << 3) | (1 << 0));

	/* Disable IRQ12 storm FIXME: bad comment */
	pci_mod_config8(dev, 0x51, (1 << 2), 0);

	pci_write_config8(dev, 0x4c, (1 << 6));

	/* FIXME: Do we really need this? SeaBIOS/linux runs fine without it.
	 * Is this something the payload/OS should do, or is it safe for us to
	 * do it? */
	/* Get the IRQs up and running */
	setup_i8259();

	vx900_lpc_dma_setup(dev);

	/* The IOAPIC is special, and we treat it separately */
	vx900_lpc_ioapic_setup(dev);
}

static void vx900_lpc_init(struct device *dev)
{
	vx900_lpc_interrupt_stuff(dev);
	vx900_lpc_misc_stuff(dev);
	dump_pci_device(dev);
}

static void vx900_lpc_read_resources(struct device *dev)
{
	struct resource *res;
	pci_dev_read_resources(dev);

	/* MMIO space */
	res = new_resource(dev, VX900_MMCONFIG_MBAR);
	res->size = 0x1000;
	res->align = 12;
	res->gran = 12;
	res->limit = 0xffffffff;
	res->flags = IORESOURCE_MEM | IORESOURCE_RESERVE;

	/* SPI controller */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->size = 0x8;
	res->align = 12;
	res->gran = 12;
	res->limit = 0xffffffff;
	res->flags = IORESOURCE_MEM | IORESOURCE_RESERVE;
}

static void vx900_lpc_set_resources(struct device *dev)
{
	struct resource *mmio, *spi;
        u32 reg;

	mmio = find_resource(dev, VX900_MMCONFIG_MBAR);
	if (mmio) {
		report_resource_stored(dev, mmio, "<mmconfig>");
		mmio->flags |= IORESOURCE_STORED;
		reg = pci_read_config32(dev, VX900_MMCONFIG_MBAR);
		reg &= 0xff000000;
		reg |= mmio->base >> 8;
		pci_write_config32(dev, VX900_MMCONFIG_MBAR, reg);

		spi = find_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
		if (spi) {
			report_resource_stored(dev, spi, "<spi>");
			spi->flags |= IORESOURCE_STORED;
			/* Set base and the enable bit. */
			((u32*)(uintptr_t)mmio->base)[0] = (spi->base | 0x01);
		}
	}
	pci_dev_set_resources(dev);
}

static struct device_operations vx900_lpc_ops = {
	.read_resources = vx900_lpc_read_resources,
	.set_resources = vx900_lpc_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = vx900_lpc_init,
	.scan_bus = scan_lpc_bus,
};

static const struct pci_driver lpc_driver __pci_driver = {
	.ops = &vx900_lpc_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_VX900_LPC,
};

#if IS_ENABLED(CONFIG_PIRQ_ROUTE)
void pirq_assign_irqs(const u8 * pirq)
{
	struct device *lpc;

	lpc = dev_find_device(PCI_VENDOR_ID_VIA,
			      PCI_DEVICE_ID_VIA_VX900_LPC, 0);

	/* Take care of INTA -> INTD */
	pci_mod_config8(lpc, 0x55, (0xf << 4), pirq[0] << 4);
	pci_write_config8(lpc, 0x56, pirq[1] | (pirq[2] << 4));
	pci_write_config8(lpc, 0x57, pirq[3] << 4);

	/* Enable INTE -> INTH to be on separate IRQs */
	pci_mod_config8(lpc, 0x46, 0, 1 << 4);
	/* Now do INTE -> INTH */
	pci_write_config8(lpc, 0x44, pirq[4] | (pirq[5] << 4));
	pci_write_config8(lpc, 0x45, pirq[6] | (pirq[7] << 4));
}
#endif
