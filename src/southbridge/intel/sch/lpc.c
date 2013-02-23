/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2009-2010 iWave Systems
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
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
#include <arch/io.h>
#include <arch/ioapic.h>
#include "chip.h"

/* SCH LPC defines */
#define SCH_ACPI_CTL	0x58
#define SCH_SIRQ_CTL	0x68
#define PIRQA_ROUT	0x60
#define PIRQB_ROUT	0x61
#define PIRQC_ROUT	0x62
#define PIRQD_ROUT	0x63
#define PIRQE_ROUT	0x64
#define PIRQF_ROUT	0x65
#define PIRQG_ROUT	0x66
#define PIRQH_ROUT	0x67

typedef struct southbridge_intel_sch_config config_t;

/* PIRQ[n]_ROUT[3:0] - PIRQ Routing Control
 * 0x00 - 0000 = Reserved
 * 0x01 - 0001 = Reserved
 * 0x02 - 0010 = Reserved
 * 0x03 - 0011 = IRQ3
 * 0x04 - 0100 = IRQ4
 * 0x05 - 0101 = IRQ5
 * 0x06 - 0110 = IRQ6
 * 0x07 - 0111 = IRQ7
 * 0x08 - 1000 = Reserved
 * 0x09 - 1001 = IRQ9
 * 0x0A - 1010 = IRQ10
 * 0x0B - 1011 = IRQ11
 * 0x0C - 1100 = IRQ12
 * 0x0D - 1101 = Reserved
 * 0x0E - 1110 = IRQ14
 * 0x0F - 1111 = IRQ15
 * PIRQ[n]_ROUT[7] - PIRQ Routing Control
 * 0x80 - The PIRQ is not routed.
 */

#define PIRQA 0x03
#define PIRQB 0x05
#define PIRQC 0x06
#define PIRQD 0x07
#define PIRQE 0x09
#define PIRQF 0x0A
#define PIRQG 0x0B
#define PIRQH 0x0C

static void sch_pirq_init(device_t dev)
{
	device_t irq_dev;

	/* Get the chip configuration */
	config_t *config = dev->chip_info;

	pci_write_config8(dev, PIRQA_ROUT, config->pirqa_routing);
	pci_write_config8(dev, PIRQB_ROUT, config->pirqb_routing);
	pci_write_config8(dev, PIRQC_ROUT, config->pirqc_routing);
	pci_write_config8(dev, PIRQD_ROUT, config->pirqd_routing);

	pci_write_config8(dev, PIRQE_ROUT, config->pirqe_routing);
	pci_write_config8(dev, PIRQF_ROUT, config->pirqf_routing);
	pci_write_config8(dev, PIRQG_ROUT, config->pirqg_routing);
	pci_write_config8(dev, PIRQH_ROUT, config->pirqh_routing);

	/* Eric Biederman once said we should let the OS do this.
	 * I am not so sure anymore he was right.
	 */

	for (irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {
		u8 int_pin = 0, int_line = 0;

		if (!irq_dev->enabled || irq_dev->path.type != DEVICE_PATH_PCI)
			continue;

		int_pin = pci_read_config8(irq_dev, PCI_INTERRUPT_PIN);

		switch (int_pin) {
		case 1: /* INTA# */
			int_line = config->pirqa_routing;
			break;
		case 2: /* INTB# */
			int_line = config->pirqb_routing;
			break;
		case 3: /* INTC# */
			int_line = config->pirqc_routing;
			break;
		case 4: /* INTD# */
			int_line = config->pirqd_routing;
			break;
		}

		if (!int_line)
			continue;

		pci_write_config8(irq_dev, PCI_INTERRUPT_LINE, int_line);
	}
}

static void sch_fixups(struct device *dev)
{
	u32 rcba_base;

	/* This needs to happen after PCI enumeration. */
	/* RCBA32(0x1d40) |= 1; */
	rcba_base = pci_read_config32(dev, 0xF0);

	/* Remove the enable bit. */
	rcba_base = rcba_base >> 1;
	rcba_base = rcba_base << 1;
	*((volatile u32 *)(rcba_base +0x104)) &= 0xFF00FFFF;
}

static void lpc_init(struct device *dev)
{
	printk(BIOS_DEBUG, "SCH: lpc_init\n");

	/* Setup the PIRQ. */
	sch_pirq_init(dev);
	pci_write_config8(dev, SCH_SIRQ_CTL,0x80);
	sch_fixups(dev);
}

static void sch_lpc_read_resources(device_t dev)
{
	struct resource *res;

	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add an extra subtractive resource for both memory and I/O. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->base = 0;
	res->size = 0xe000;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->base = 0xff800000;
	res->size = 0x00800000; /* 8 MB for flash */
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 3);
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static struct pci_operations pci_ops = {
	.set_subsystem = set_subsystem,
};

static struct device_operations device_ops = {
	.read_resources		= sch_lpc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= lpc_init,
	.scan_bus		= scan_static_bus,
	.ops_pci		= &pci_ops,
};

/* SCH LPC Interface */
static const struct pci_driver sch_lpc __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x8119,
};
