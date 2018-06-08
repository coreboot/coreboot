/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 - 2017 Intel Corporation.
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
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include <arch/acpi.h>
#include <cpu/cpu.h>
#include <cpu/x86/smm.h>
#include <bootstate.h>

#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/iomap.h>
#include <soc/pcr.h>
#include <soc/p2sb.h>
#include <soc/acpi.h>

#include "chip.h"

/* PCH-LP redirection entries */
#define PCH_LP_REDIR_ETR 120

/**
 * Set miscellaneous static southbridge features.
 *
 * @param dev PCI device with I/O APIC control registers
 */
static void pch_enable_ioapic(struct device *dev)
{
	u32 reg32;

	set_ioapic_id((void *)IO_APIC_ADDR, IO_APIC0);

	/* affirm full set of redirection table entries ("write once") */
	reg32 = io_apic_read((void *)IO_APIC_ADDR, 0x01);

	reg32 &= ~0x00ff0000;
	reg32 |= (PCH_LP_REDIR_ETR - 1) << 16;

	io_apic_write((void *)IO_APIC_ADDR, 0x01, reg32);

	/*
	 * Select Boot Configuration register (0x03) and
	 * use Processor System Bus (0x01) to deliver interrupts.
	 */
	io_apic_write((void *)IO_APIC_ADDR, 0x03, 0x01);
}

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

static void pch_pirq_init(struct device *dev)
{
	struct device *irq_dev;
	/* Get the chip configuration */
	config_t *config = dev->chip_info;

	/* Initialize PIRQ Routings */
	write8((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIRQA_ROUT),
	       config->pirqa_routing);
	write8((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIRQB_ROUT),
	       config->pirqb_routing);
	write8((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIRQC_ROUT),
	       config->pirqc_routing);
	write8((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIRQD_ROUT),
	       config->pirqd_routing);

	write8((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIRQE_ROUT),
	       config->pirqe_routing);
	write8((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIRQF_ROUT),
	       config->pirqf_routing);
	write8((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIRQG_ROUT),
	       config->pirqg_routing);
	write8((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIRQH_ROUT),
	       config->pirqh_routing);

	/* Initialize device's Interrupt Routings */
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR00),
		config->ir00_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR01),
		config->ir01_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR02),
		config->ir02_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR03),
		config->ir03_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR04),
		config->ir04_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR05),
		config->ir05_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR06),
		config->ir06_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR07),
		config->ir07_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR08),
		config->ir08_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR09),
		config->ir09_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR10),
		config->ir10_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR11),
		config->ir11_routing);
	write16((void *)PCH_PCR_ADDRESS(PID_ITSS, PCR_ITSS_PIR12),
		config->ir12_routing);

	/* Initialize device's Interrupt Polarity Control */
	write32((void *)PCH_PCR_ADDRESS(PID_ITSS, PCH_PCR_ITSS_IPC0),
		config->ipc0);
	write32((void *)PCH_PCR_ADDRESS(PID_ITSS, PCH_PCR_ITSS_IPC1),
		config->ipc1);
	write32((void *)PCH_PCR_ADDRESS(PID_ITSS, PCH_PCR_ITSS_IPC2),
		config->ipc2);
	write32((void *)PCH_PCR_ADDRESS(PID_ITSS, PCH_PCR_ITSS_IPC3),
		config->ipc3);

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

static void pci_p2sb_read_resources(struct device *dev)
{
	struct resource *res;

	/* Add MMIO resource
	 * Use 0xda as an unused index for PCR BAR.
	 */
	res = new_resource(dev, 0xda);
	res->base = DEFAULT_PCR_BASE;
	res->size = 16 * 1024 * 1024; /* 16MB PCR config space */
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_STORED |
		     IORESOURCE_ASSIGNED;
	printk(BIOS_DEBUG,
	       "Adding P2SB PCR config space BAR 0x%08lx-0x%08lx.\n",
	       (unsigned long)(res->base),
	       (unsigned long)(res->base + res->size));

	/* Add MMIO resource
	 * Use 0xdb as an unused index for IOAPIC.
	 */
	res = new_resource(dev, 0xdb); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void pch_enable_serial_irqs(struct device *dev)
{
	/* Set packet length and toggle silent mode bit for one frame. */
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (1 << 6) | ((21 - 17) << 2) | (0 << 0));
#if !IS_ENABLED(CONFIG_SERIRQ_CONTINUOUS_MODE)
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (0 << 6) | ((21 - 17) << 2) | (0 << 0));
#endif
}

static void lpc_init(struct device *dev)
{
	printk(BIOS_DEBUG, "pch: lpc_init\n");

	/* Get the base address */

	/* Set the value for PCI command register. */
	pci_write_config16(dev, PCI_COMMAND,
			   PCI_COMMAND_SPECIAL | PCI_COMMAND_MASTER |
				   PCI_COMMAND_MEMORY | PCI_COMMAND_IO);

	/* Serial IRQ initialization. */
	pch_enable_serial_irqs(dev);

	/* IO APIC initialization. */
	pch_enable_ioapic(dev);

	/* Setup the PIRQ. */
	pch_pirq_init(dev);
}

static void pch_lpc_add_mmio_resources(struct device *dev) { /* TODO */ }

static void pch_lpc_add_io_resources(struct device *dev)
{
	struct resource *res;
	u8 io_index = 0;

	/* Add an extra subtractive resource for both memory and I/O. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
	res->base = 0;
	res->size = 0x1000;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
	res->base = 0xff000000;
	res->size = 0x01000000; /* 16 MB for flash */
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void lpc_read_resources(struct device *dev)
{
	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add non-standard MMIO resources. */
	pch_lpc_add_mmio_resources(dev);

	/* Add IO resources. */
	pch_lpc_add_io_resources(dev);

	/* Add MMIO resource for IOAPIC. */
	pci_p2sb_read_resources(dev);
}

static void pch_decode_init(struct device *dev) { /* TODO */ }

static void lpc_enable_resources(struct device *dev)
{
	pch_decode_init(dev);
	pci_dev_enable_resources(dev);
}

/* Set bit in Function Disable register to hide this device */
static void pch_hide_devfn(uint32_t devfn) { /* TODO */ }

void southcluster_enable_dev(struct device *dev)
{
	u32 reg32;

	if (!dev->enabled) {
		printk(BIOS_DEBUG, "%s: Disabling device\n", dev_path(dev));

		/* Ensure memory, io, and bus master are all disabled */
		reg32 = pci_read_config32(dev, PCI_COMMAND);
		reg32 &= ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY |
			   PCI_COMMAND_IO);
		pci_write_config32(dev, PCI_COMMAND, reg32);

		/* Hide this device if possible */
		pch_hide_devfn(dev->path.pci.devfn);
	} else {
		/* Enable SERR */
		reg32 = pci_read_config32(dev, PCI_COMMAND);
		reg32 |= PCI_COMMAND_SERR;
		pci_write_config32(dev, PCI_COMMAND, reg32);
	}
}

static struct device_operations device_ops = {
	.read_resources = lpc_read_resources,
	.set_resources = pci_dev_set_resources,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.acpi_inject_dsdt_generator = southcluster_inject_dsdt,
	.write_acpi_tables = southcluster_write_acpi_tables,
#endif
	.enable_resources = lpc_enable_resources,
	.init = lpc_init,
	.enable = southcluster_enable_dev,
	.scan_bus = scan_lpc_bus,
	.ops_pci = &soc_pci_ops,
};

static const struct pci_driver lpc_driver __pci_driver = {
	.ops = &device_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = LPC_DEVID,
};

static void finalize_chipset(void *unused)
{
	printk(BIOS_DEBUG, "Finalizing SMM.\n");
	outb(APM_CNT_FINALIZE, APM_CNT);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, finalize_chipset, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, finalize_chipset, NULL);
