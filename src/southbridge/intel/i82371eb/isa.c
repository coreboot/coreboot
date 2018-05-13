/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
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
 */

#include <stdint.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/isa-dma.h>
#include <pc80/mc146818rtc.h>
#include <arch/ioapic.h>
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
#include <arch/acpi.h>
#include <arch/acpigen.h>
#endif
#include "i82371eb.h"

#if IS_ENABLED(CONFIG_IOAPIC)
static void enable_intel_82093aa_ioapic(void)
{
	u16 reg16;
	u32 reg32;
	u8 ioapic_id = 2;
	volatile u32 *ioapic_index = (volatile u32 *)(IO_APIC_ADDR);
	volatile u32 *ioapic_data = (volatile u32 *)(IO_APIC_ADDR + 0x10);
	struct device *dev;

	dev = dev_find_device(PCI_VENDOR_ID_INTEL,
			      PCI_DEVICE_ID_INTEL_82371AB_ISA, 0);

	/* Enable IOAPIC. */
	reg16 = pci_read_config16(dev, XBCS);
	reg16 |= (1 << 8); /* APIC Chip Select */
	pci_write_config16(dev, XBCS, reg16);

	/* Set the IOAPIC ID. */
	*ioapic_index = 0;
	*ioapic_data = ioapic_id << 24;

	/* Read back and verify the IOAPIC ID. */
	*ioapic_index = 0;
	reg32 = (*ioapic_data >> 24) & 0x0f;
	printk(BIOS_DEBUG, "IOAPIC ID = %x\n", reg32);
	if (reg32 != ioapic_id)
		die("IOAPIC error!\n");
}
#endif

static void isa_init(struct device *dev)
{
	u32 reg32;

	/* Initialize the real time clock (RTC). */
	cmos_init(0);

	/*
	 * Enable special cycles, needed for soft poweroff.
	 */
	reg32 = pci_read_config16(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_SPECIAL;
	pci_write_config16(dev, PCI_COMMAND, reg32);

	/*
	 * The PIIX4 can support the full ISA bus, or the Extended I/O (EIO)
	 * bus, which is a subset of ISA. We select the full ISA bus here.
	 */
	reg32 = pci_read_config32(dev, GENCFG);
	reg32 |= ISA;	/* Select ISA, not EIO. */
	pci_write_config16(dev, GENCFG, reg32);

	/* Initialize ISA DMA. */
	isa_dma_init();

#if IS_ENABLED(CONFIG_IOAPIC)
	/*
	 * Unlike most other southbridges the 82371EB doesn't have a built-in
	 * IOAPIC. Instead, 82371EB-based boards that support multiple CPUs
	 * have a discrete IOAPIC (Intel 82093AA) soldered onto the board.
	 *
	 * Thus, we can/must only enable the IOAPIC if it actually exists,
	 * i.e. the respective mainboard does "select IOAPIC".
	 */
	enable_intel_82093aa_ioapic();
#endif
}

static void sb_read_resources(struct device *dev)
{
	struct resource *res;

	pci_dev_read_resources(dev);

	res = new_resource(dev, 1);
	res->base = 0x0UL;
	res->size = 0x1000UL;
	res->limit = 0xffffUL;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 2);
	res->base = 0xff800000UL;
	res->size = 0x00800000UL; /* 8 MB for flash */
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED |
		IORESOURCE_RESERVE;

#if IS_ENABLED(CONFIG_IOAPIC)
	res = new_resource(dev, 3); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED |
		IORESOURCE_RESERVE;
#endif
}

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
static void southbridge_acpi_fill_ssdt_generator(struct device *device)
{
	acpigen_write_mainboard_resources("\\_SB.PCI0.MBRS", "_CRS");
	generate_cpu_entries(device);
}
#endif

static const struct device_operations isa_ops = {
	.read_resources		= sb_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.write_acpi_tables      = acpi_write_hpet,
	.acpi_fill_ssdt_generator = southbridge_acpi_fill_ssdt_generator,
#endif
	.init			= isa_init,
	.scan_bus		= scan_lpc_bus,	/* TODO: Needed? */
	.enable			= 0,
	.ops_pci		= 0, /* No subsystem IDs on 82371EB! */
};

static const struct pci_driver isa_driver __pci_driver = {
	.ops	= &isa_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82371AB_ISA,
};

static const struct pci_driver isa_SB_driver __pci_driver = {
	.ops	= &isa_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82371SB_ISA,
};
