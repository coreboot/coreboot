/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Linux Networx
 * Copyright (C) 2003 SuSE Linux AG
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pnp.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include <cpu/x86/lapic.h>
#include <arch/acpi.h>
#include <stdlib.h>
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
#include <arch/acpi.h>
#include <arch/acpigen.h>
#endif
#include <cpu/amd/powernow.h>
#include "mcp55.h"

#define NMI_OFF	0

// 0x7a or e3
#define PREVIOUS_POWER_STATE	0x7A

#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define SLOW_CPU_OFF		0
#define SLOW_CPU__ON		1

#ifndef CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif

static void lpc_common_init(device_t dev, int master)
{
	u8 byte;
	void *ioapic_base;

	/* IOAPIC initialization. */
	byte = pci_read_config8(dev, 0x74);
	byte |= (1 << 0); /* Enable IOAPIC. */
	pci_write_config8(dev, 0x74, byte);
	ioapic_base = (void *)pci_read_config32(dev, PCI_BASE_ADDRESS_1); /* 0x14 */

	if (master)
		setup_ioapic(ioapic_base, 0);
	else
		clear_ioapic(ioapic_base);
}

static void lpc_slave_init(device_t dev)
{
	lpc_common_init(dev, 0);
}

static void enable_hpet(struct device *dev)
{
	unsigned long hpet_address;

	pci_write_config32(dev, 0x44, CONFIG_HPET_ADDRESS|1);
	hpet_address=pci_read_config32(dev, 0x44) & 0xfffffffe;
	printk(BIOS_DEBUG, "enabling HPET @0x%lx\n", hpet_address);
}

static void lpc_init(device_t dev)
{
	u8 byte, byte_old;
	int on, nmi_option;

	lpc_common_init(dev, 1);

	/* power after power fail */

#if 1
	on = CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL;
	get_option(&on, "power_on_after_fail");
	byte = pci_read_config8(dev, PREVIOUS_POWER_STATE);
	byte &= ~0x40;
	if (!on)
		byte |= 0x40;
	pci_write_config8(dev, PREVIOUS_POWER_STATE, byte);
	printk(BIOS_INFO, "set power %s after power fail\n", on ? "on" : "off");
#endif
	/* Throttle the CPU speed down for testing. */
	on = SLOW_CPU_OFF;
	get_option(&on, "slow_cpu");
	if (on) {
		u16 pm10_bar;
		u32 dword;
		pm10_bar = (pci_read_config16(dev, 0x60) & 0xff00);
		outl(((on << 1) + 0x10), (pm10_bar + 0x10));
		dword = inl(pm10_bar + 0x10);
		on = 8 - on;
		printk(BIOS_DEBUG, "Throttling CPU %2d.%1.1d percent.\n",
		       (on * 12) + (on >> 1), (on & 1) * 5);
	}

	/* Enable error reporting. */
	/* Set up sync flood detected. */
	byte = pci_read_config8(dev, 0x47);
	byte |= (1 << 1);
	pci_write_config8(dev, 0x47, byte);

	/* Set up NMI on errors. */
	byte = inb(0x70); /* RTC70 */
	byte_old = byte;
	nmi_option = NMI_OFF;
	get_option(&nmi_option, "nmi");
	if (nmi_option)
		byte &= ~(1 << 7); /* Set NMI. */
	else
		byte |= (1 << 7); /* Can't mask NMI from PCI-E and NMI_NOW. */
	if (byte != byte_old)
		outb(byte, 0x70);

	/* Initialize the real time clock. */
	cmos_init(0);

	/* Initialize ISA DMA. */
	isa_dma_init();

	/* Initialize the High Precision Event Timers (HPET). */
	enable_hpet(dev);
}

static void mcp55_lpc_read_resources(device_t dev)
{
	struct resource *res;

	/* Get the normal PCI resources of this device. */
	/* We got one for APIC, or one more for TRAP. */
	pci_dev_read_resources(dev);

	/* Add an extra subtractive resource for both memory and I/O. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->base = 0;
	res->size = 0x1000;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->base = 0xff800000;
	res->size = 0x00800000; /* 8 MB for flash */
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

/**
 * Enable resources for children devices.
 *
 * @param dev The device whose children's resources are to be enabled.
 */
static void mcp55_lpc_enable_childrens_resources(device_t dev)
{
	u32 reg, reg_var[4];
	int i, var_num = 0;
	struct bus *link;

	reg = pci_read_config32(dev, 0xa0);

	for (link = dev->link_list; link; link = link->next) {
		device_t child;
		for (child = link->children; child; child = child->sibling) {
			if (child->enabled && (child->path.type == DEVICE_PATH_PNP)) {
				struct resource *res;
				for (res = child->resource_list; res; res = res->next) {
					unsigned long base, end; /* Don't need long long. */
					if (!(res->flags & IORESOURCE_IO))
						continue;
					base = res->base;
					end = resource_end(res);
					printk(BIOS_DEBUG, "mcp55 lpc decode:%s, base=0x%08lx, end=0x%08lx\n",dev_path(child),base, end);
					switch (base) {
					case 0x3f8: /* COM1 */
						reg |= (1 << 0);
						break;
					case 0x2f8: /* COM2 */
						reg |= (1 << 1);
						break;
					case 0x378: /* Parallel 1 */
						reg |= (1 << 24);
						break;
					case 0x3f0: /* FD0 */
						reg |= (1 << 20);
						break;
					case 0x220: /* Audio 0 */
						reg |= (1 << 8);
						break;
					case 0x300: /* Midi 0 */
						reg |= (1 << 12);
						break;
					}
					if ((base == 0x290)
					    || (base >= 0x400)) {
						/* Only 4 var; compact them? */
						if (var_num >= 4)
							continue;
						reg |= (1 << (28 + var_num));
						reg_var[var_num++] = (base & 0xffff) | ((end & 0xffff) << 16);
					}
				}
			}
		}
	}
	pci_write_config32(dev, 0xa0, reg);
	for (i = 0; i < var_num; i++)
		pci_write_config32(dev, 0xa8 + i * 4, reg_var[i]);
}

static void mcp55_lpc_enable_resources(device_t dev)
{
	pci_dev_enable_resources(dev);
	mcp55_lpc_enable_childrens_resources(dev);
}

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
static void southbridge_acpi_fill_ssdt_generator(device_t device)
{
	amd_generate_powernow(0, 0, 0);
}
#endif

static struct device_operations lpc_ops = {
	.read_resources   = mcp55_lpc_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = mcp55_lpc_enable_resources,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.acpi_fill_ssdt_generator = southbridge_acpi_fill_ssdt_generator,
	.write_acpi_tables = acpi_write_hpet,
#endif
	.init             = lpc_init,
	.scan_bus         = scan_lpc_bus,
	.ops_pci          = &mcp55_pci_ops,
};
static const unsigned short lpc_ids[] = {
	PCI_DEVICE_ID_NVIDIA_MCP55_LPC,
	PCI_DEVICE_ID_NVIDIA_MCP55_PRO,
	PCI_DEVICE_ID_NVIDIA_MCP55_LPC_2,
	PCI_DEVICE_ID_NVIDIA_MCP55_LPC_3,
	PCI_DEVICE_ID_NVIDIA_MCP55_LPC_4,
	PCI_DEVICE_ID_NVIDIA_MCP55_LPC_5,
	PCI_DEVICE_ID_NVIDIA_MCP55_LPC_6,
	0
};
static const struct pci_driver lpc_driver __pci_driver = {
	.ops	= &lpc_ops,
	.vendor	= PCI_VENDOR_ID_NVIDIA,
	.devices = lpc_ids,
};

static struct device_operations lpc_slave_ops = {
	.read_resources   = mcp55_lpc_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.write_acpi_tables      = acpi_write_hpet,
#endif
	.init             = lpc_slave_init,
	.ops_pci          = &mcp55_pci_ops,
};

static const struct pci_driver lpc_driver_slave __pci_driver = {
	.ops	= &lpc_slave_ops,
	.vendor	= PCI_VENDOR_ID_NVIDIA,
	.device	= PCI_DEVICE_ID_NVIDIA_MCP55_SLAVE,
};
