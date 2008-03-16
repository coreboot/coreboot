/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Linux Networx
 * Copyright (C) 2008 Arastra, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

/* This code is based on src/southbridge/intel/esb6300/esb6300_lpc.c */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <arch/io.h>
#include "i3100.h"

#define ACPI_BAR 0x40
#define GPIO_BAR 0x48
#define RCBA 0xf0

#define NMI_OFF 0
#define MAINBOARD_POWER_OFF 0
#define MAINBOARD_POWER_ON  1

#ifndef MAINBOARD_POWER_ON_AFTER_FAIL
#define MAINBOARD_POWER_ON_AFTER_FAIL MAINBOARD_POWER_ON
#endif

#define ALL		(0xff << 24)
#define NONE		(0)
#define DISABLED	(1 << 16)
#define ENABLED		(0 << 16)
#define TRIGGER_EDGE	(0 << 15)
#define TRIGGER_LEVEL	(1 << 15)
#define POLARITY_HIGH	(0 << 13)
#define POLARITY_LOW	(1 << 13)
#define PHYSICAL_DEST	(0 << 11)
#define LOGICAL_DEST	(1 << 11)
#define ExtINT		(7 << 8)
#define NMI		(4 << 8)
#define SMI		(2 << 8)
#define INT		(1 << 8)

static void setup_ioapic(device_t dev)
{
	int i;
	u32 value_low, value_high;
	u32 ioapic_base = 0xfec00000;
	volatile u32 *l;
	u32 interrupts;
	struct resource *res;

	/* Enable IO APIC */
	res = find_resource(dev, RCBA);
	if (!res) {
		return;
	}
	*((u8 *)(res->base + 0x31ff)) |= (1 << 0);

	l = (u32 *) ioapic_base;

	l[0] = 0x01;
	interrupts = (l[04] >> 16) & 0xff;
	for (i = 0; i < interrupts; i++) {
		l[0] = (i * 2) + 0x10;
		l[4] = DISABLED;
		value_low = l[4];
		l[0] = (i * 2) + 0x11;
		l[4] = NONE; /* Should this be an address? */
		value_high = l[4];
		if (value_low == 0xffffffff) {
			printk_warning("%d IO APIC not responding.\n",
				dev_path(dev));
			return;
		}
	}

	/* Put the APIC in virtual wire mode */
	l[0] = 0x10;
	l[4] = ENABLED | TRIGGER_EDGE | POLARITY_HIGH | PHYSICAL_DEST | ExtINT;
}

#define SERIRQ_CNTL 0x64
static void i3100_enable_serial_irqs(device_t dev)
{
	/* set packet length and toggle silent mode bit */
	pci_write_config8(dev, SERIRQ_CNTL, (1 << 7)|(1 << 6)|((21 - 17) << 2)|(0 << 0));
	pci_write_config8(dev, SERIRQ_CNTL, (1 << 7)|(0 << 6)|((21 - 17) << 2)|(0 << 0));
}

typedef struct southbridge_intel_i3100_config config_t;

static void set_i3100_gpio_use_sel(
	device_t dev, struct resource *res, config_t *config)
{
	u32 gpio_use_sel, gpio_use_sel2;

	gpio_use_sel  = 0x1b0ce7c3;
	gpio_use_sel2 = 0x00000107;
	outl(gpio_use_sel,  res->base + 0x00);
	outl(gpio_use_sel2, res->base + 0x30);
}

static void set_i3100_gpio_direction(
	device_t dev, struct resource *res, config_t *config)
{
	u32 gpio_io_sel, gpio_io_sel2;

	gpio_io_sel  = 0xed00ffff;
	gpio_io_sel2 = 0x00000307;
	outl(gpio_io_sel,  res->base + 0x04);
	outl(gpio_io_sel2, res->base + 0x34);
}

static void set_i3100_gpio_level(
	device_t dev, struct resource *res, config_t *config)
{
	u32 gpio_lvl, gpio_lvl2;
	u32 gpio_blink;

	gpio_lvl   = 0x00030000;
	gpio_blink = 0x00000000;
	gpio_lvl2  = 0x00000300;
	outl(gpio_lvl,   res->base + 0x0c);
	outl(gpio_blink, res->base + 0x18);
	outl(gpio_lvl2,  res->base + 0x38);
}

static void set_i3100_gpio_inv(
	device_t dev, struct resource *res, config_t *config)
{
	u32 gpio_inv;

	gpio_inv   = 0x00006000;
	outl(gpio_inv,   res->base + 0x2c);
}

static void i3100_pirq_init(device_t dev)
{
	config_t *config;

	/* Get the chip configuration */
	config = dev->chip_info;

	if(config->pirq_a_d) {
		pci_write_config32(dev, 0x60, config->pirq_a_d);
	}
	if(config->pirq_e_h) {
		pci_write_config32(dev, 0x68, config->pirq_e_h);
	}
}


static void i3100_gpio_init(device_t dev)
{
	struct resource *res;
	config_t *config;

	/* Skip if I don't have any configuration */
	if (!dev->chip_info) {
		return;
	}
	/* The programmer is responsible for ensuring
	 * a valid gpio configuration.
	 */

	/* Get the chip configuration */
	config = dev->chip_info;
	/* Find the GPIO bar */
	res = find_resource(dev, GPIO_BAR);
	if (!res) {
		return;
	}

	/* Set the use selects */
	set_i3100_gpio_use_sel(dev, res, config);

	/* Set the IO direction */
	set_i3100_gpio_direction(dev, res, config);

	/* Setup the input inverters */
	set_i3100_gpio_inv(dev, res, config);

	/* Set the value on the GPIO output pins */
	set_i3100_gpio_level(dev, res, config);

}


static void lpc_init(struct device *dev)
{
	u8 byte;
	int pwr_on = MAINBOARD_POWER_ON_AFTER_FAIL;

	setup_ioapic(dev);

	/* Decode 0xffc00000 - 0xffffffff to fwh idsel 0 */
	pci_write_config32(dev, 0xd0, 0x00000000);

	i3100_enable_serial_irqs(dev);

	get_option(&pwr_on, "power_on_after_fail");
	byte = pci_read_config8(dev, 0xa4);
	byte &= 0xfe;
	if (!pwr_on) {
		byte |= 1;
	}
	pci_write_config8(dev, 0xa4, byte);
	printk_info("set power %s after power fail\n", pwr_on ? "on" : "off");

	/* Set up the PIRQ */
	i3100_pirq_init(dev);

	/* Set the state of the gpio lines */
	i3100_gpio_init(dev);

	/* Initialize the real time clock */
	rtc_init(0);

	/* Initialize isa dma */
	isa_dma_init();
}

static void i3100_lpc_read_resources(device_t dev)
{
	struct resource *res;

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);

	/* Add the ACPI BAR */
	res = pci_get_resource(dev, ACPI_BAR);

	/* Add the GPIO BAR */
	res = pci_get_resource(dev, GPIO_BAR);

	/* Add an extra subtractive resource for both memory and I/O */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

	/* Add resource for RCBA */
	res = new_resource(dev, RCBA);
	res->size = 0x4000;
	res->limit = 0xffffc000;
	res->align = 14;
	res->gran = 14;
	res->flags = IORESOURCE_MEM;
}

static void i3100_lpc_enable_resources(device_t dev)
{
	u8 acpi_cntl, gpio_cntl;

	/* Enable the normal pci resources */
	pci_dev_enable_resources(dev);

	/* Enable the ACPI bar */
	acpi_cntl = pci_read_config8(dev, 0x44);
	acpi_cntl |= (1 << 4);
	pci_write_config8(dev, 0x44, acpi_cntl);

	/* Enable the GPIO bar */
	gpio_cntl = pci_read_config8(dev, 0x4c);
	gpio_cntl |= (1 << 4);
	pci_write_config8(dev, 0x4c, gpio_cntl);

	/* Enable the RCBA */
	pci_write_config32(dev, RCBA, pci_read_config32(dev, RCBA) | (1 << 0));

	enable_childrens_resources(dev);
}

static struct pci_operations lops_pci = {
	.set_subsystem = 0,
};

static struct device_operations lpc_ops  = {
	.read_resources   = i3100_lpc_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = i3100_lpc_enable_resources,
	.init             = lpc_init,
	.scan_bus         = scan_static_bus,
	.enable           = i3100_enable,
	.ops_pci          = &lops_pci,
};

static struct pci_driver lpc_driver __pci_driver = {
	.ops    = &lpc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_3100_LPC,
};
