/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
 * Copyright (C) 2017 Siemens AG
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

#include <stdint.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_def.h>
#include <pc80/mc146818rtc.h>
#include <pc80/i8254.h>
#include <pc80/i8259.h>
#include <pc80/isa-dma.h>
#include <romstage_handoff.h>
#include <soc/iomap.h>
#include <soc/irq.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <chip.h>

typedef struct soc_intel_fsp_broadwell_de_config config_t;

static inline void
add_mmio_resource(struct device *dev, int i, unsigned long addr,
		  unsigned long size)
{
	mmio_resource(dev, i, addr >> 10, size >> 10);
}

static void sc_add_mmio_resources(struct device *dev)
{
	add_mmio_resource(dev, 0xfeb0,
	                       ABORT_BASE_ADDRESS,
	                       ABORT_BASE_SIZE);
	add_mmio_resource(dev, 0xfeb8,
	                       PSEG_BASE_ADDRESS,
	                       PSEG_BASE_SIZE);
	add_mmio_resource(dev, 0xfec0,
	                       IOXAPIC1_BASE_ADDRESS,
	                       IOXAPIC1_BASE_SIZE);
	add_mmio_resource(dev, 0xfec1,
	                       IOXAPIC2_BASE_ADDRESS,
	                       IOXAPIC2_BASE_SIZE);
	add_mmio_resource(dev, 0xfed0,
	                       PCH_BASE_ADDRESS,
	                       PCH_BASE_SIZE);
	add_mmio_resource(dev, 0xfee0,
	                       LXAPIC_BASE_ADDRESS,
	                       LXAPIC_BASE_SIZE);
	add_mmio_resource(dev, 0xff00,
	                       FIRMWARE_BASE_ADDRESS,
	                       FIRMWARE_BASE_SIZE);
}

/*
 * Write PCI config space IRQ assignments.  PCI devices have the INT_LINE
 * (0x3C) and INT_PIN (0x3D) registers which report interrupt routing
 * information to operating systems and drivers.  The INT_PIN register is
 * generally read only and reports which interrupt pin A - D it uses.  The
 * INT_LINE register is configurable and reports which IRQ (generally the
 * PIC IRQs 1 - 15) it will use.  This needs to take interrupt pin swizzling
 * on devices that are downstream on a PCI bridge into account.
 *
 * This function will loop through all enabled PCI devices and program the
 * INT_LINE register with the correct PIC IRQ number for the INT_PIN that it
 * uses.  It then configures each interrupt in the pic to be level triggered.
 */
static void write_pci_config_irqs(void)
{
	struct device *irq_dev;
	struct device *targ_dev;
	uint8_t int_line = 0;
	uint8_t original_int_pin = 0;
	uint8_t new_int_pin = 0;
	uint16_t current_bdf = 0;
	uint16_t parent_bdf = 0;
	uint8_t pirq = 0;
	uint8_t device_num = 0;
	const struct broadwell_de_irq_route *ir = &global_broadwell_de_irq_route;

	if (ir == NULL) {
		printk(BIOS_WARNING, "Warning: Can't write PCI IRQ assignments because"
				" 'global_broadwell_de_irq_route' structure does not exist\n");
		return;
	}

	/*
	 * Loop through all enabled devices and program their
	 * INT_LINE, INT_PIN registers from values taken from
	 * the Interrupt Route registers in the ILB
	 */
	printk(BIOS_DEBUG, "PCI_CFG IRQ: Write PCI config space IRQ assignments\n");
	for (irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {

		if ((irq_dev->path.type != DEVICE_PATH_PCI) ||
			(!irq_dev->enabled))
			continue;

		current_bdf = irq_dev->path.pci.devfn |
			irq_dev->bus->secondary << 8;

		/*
		 * Step 1: Get the INT_PIN and device structure to look for
		 * in the pirq_data table defined in the mainboard directory.
		 */
		targ_dev = NULL;
		new_int_pin = get_pci_irq_pins(irq_dev, &targ_dev);
		if (targ_dev == NULL || new_int_pin < 1)
			continue;

		/* Get the original INT_PIN for record keeping */
		original_int_pin = pci_read_config8(irq_dev, PCI_INTERRUPT_PIN);

		parent_bdf = targ_dev->path.pci.devfn
			| targ_dev->bus->secondary << 8;
		device_num = PCI_SLOT(parent_bdf);

		if (ir->pcidev[device_num] == 0) {
			printk(BIOS_WARNING,
				"Warning: PCI Device %d does not have an IRQ entry, skipping it\n",
				device_num);
			continue;
		}

		/* Find the PIRQ that is attached to the INT_PIN this device uses */
		pirq = (ir->pcidev[device_num] >> ((new_int_pin - 1) * 4)) & 0xF;

		/* Get the INT_LINE this device/function will use */
		int_line = ir->pic[pirq];

		if (int_line != PIRQ_PIC_IRQDISABLE) {
			/* Set this IRQ to level triggered since it is used by a PCI device */
			i8259_configure_irq_trigger(int_line, IRQ_LEVEL_TRIGGERED);
			/* Set the Interrupt Line register in PCI config space */
			pci_write_config8(irq_dev, PCI_INTERRUPT_LINE, int_line);
		} else {
			/* Set the Interrupt line register as "unknown or unused" */
			pci_write_config8(irq_dev, PCI_INTERRUPT_LINE,
				PIRQ_PIC_UNKNOWN_UNUSED);
		}

		printk(BIOS_SPEW, "\tINT_PIN\t\t: %d (%s)\n",
			original_int_pin, pin_to_str(original_int_pin));
		if (parent_bdf != current_bdf)
			printk(BIOS_SPEW, "\tSwizzled to\t: %d (%s)\n",
							new_int_pin, pin_to_str(new_int_pin));
		printk(BIOS_SPEW, "\tPIRQ\t\t: %c\n"
						"\tINT_LINE\t: 0x%X (IRQ %d)\n",
						'A' + pirq, int_line, int_line);
	}
	printk(BIOS_DEBUG, "PCI_CFG IRQ: Finished writing PCI config space IRQ assignments\n");
}

static void sc_pirq_init(struct device *dev)
{
	int i;
	const uint8_t *pirq = global_broadwell_de_irq_route.pic;
	printk(BIOS_DEBUG, "Programming PIRQ[A-H] Routing Control Register\n");

	for (i = 0; i < 8; i++) {
		pci_write_config8(dev, (i < 4) ? (PIRQ_RCR1+i) : (PIRQ_RCR2+i-4), pirq[i]);
		printk(BIOS_DEBUG, "  PIRQ[%c]: %.2x\n"
			, 'A'+i
			, pci_read_config8(dev, (i < 4) ? (PIRQ_RCR1+i) : (PIRQ_RCR2+i-4))
			);
	}
}

static void sc_add_io_resources(struct device *dev)
{
	struct resource *res;
	u8 io_index = 0;

	/*
	 * Add the default claimed IO range for the LPC device
	 * and mark it as subtractive decode.
	 */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
	res->base = LPC_DEFAULT_IO_RANGE_LOWER;
	res->size = LPC_DEFAULT_IO_RANGE_UPPER - LPC_DEFAULT_IO_RANGE_LOWER;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* Add the resource for GPIOs */
	res = new_resource(dev, GPIO_BASE_ADR_OFFSET);
	res->base = GPIO_BASE_ADDRESS;
	res->size = GPIO_BASE_SIZE;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
	/* There is a separated enable-bit in GPIO_CTRL-register. It was set
	 * already in romstage but FSP was active in the meantime and could have
	 * cleared it. Set it here again to enable allocated IO-space for sure.
	 */
	pci_write_config8(dev, GPIO_CTRL_OFFSET, GPIO_DECODE_ENABLE);
}

static void sc_read_resources(struct device *dev)
{
	pci_dev_read_resources(dev);
	sc_add_mmio_resources(dev);
	sc_add_io_resources(dev);
}

static void sc_init(struct device *dev)
{
	printk(BIOS_DEBUG, "soc: southcluster_init\n");

	/* Set the value for PCI command register. */
	pci_write_config16(dev, PCI_COMMAND,
		PCI_COMMAND_IO | PCI_COMMAND_MEMORY |
		PCI_COMMAND_MASTER | PCI_COMMAND_SPECIAL);

	/* Program Serial IRQ register. */
	pci_write_config8(dev, SIRQ_CNTL, SIRQ_EN | SIRQ_MODE_CONT);
	if (!IS_ENABLED(CONFIG_SERIRQ_CONTINUOUS_MODE)) {
		/* If SERIRQ have to operate in quiet mode, it should have been
		   run in continuous mode for at least one frame first. Use I/O
		   access to achieve the delay of at least one LPC cycle. */
		outb(inb(0x80), 0x80);
		pci_write_config8(dev, SIRQ_CNTL, SIRQ_EN | SIRQ_MODE_QUIET);
	}

	sc_pirq_init(dev);
	write_pci_config_irqs();
	isa_dma_init();
	setup_i8259();
	setup_i8254();
}

/*
 * Common code for the south cluster devices.
 */
void southcluster_enable_dev(struct device *dev)
{
	uint32_t reg32;

	if (!dev->enabled) {
		int slot = PCI_SLOT(dev->path.pci.devfn);
		int func = PCI_FUNC(dev->path.pci.devfn);
		printk(BIOS_DEBUG, "%s: Disabling device: %02x.%01x\n",
		       dev_path(dev), slot, func);

		/* Ensure memory, io, and bus master are all disabled */
		reg32 = pci_read_config32(dev, PCI_COMMAND);
		reg32 &= ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
		pci_write_config32(dev, PCI_COMMAND, reg32);
	}
}

#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
static const char *lpc_acpi_name(const struct device *dev)
{
	if (dev->path.pci.devfn == LPC_DEV_FUNC)
		return "LPC0";
	else
		return NULL;
}
#endif

static struct device_operations device_ops = {
	.read_resources   = sc_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = NULL,
	.init             = sc_init,
	.enable           = southcluster_enable_dev,
	.scan_bus         = scan_lpc_bus,
	.ops_pci          = &soc_pci_ops,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.acpi_name        = lpc_acpi_name,
#endif
};

static const struct pci_driver southcluster __pci_driver = {
	.ops    = &device_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = LPC_DEVID,
};

static const struct pci_driver southcluster_es2 __pci_driver = {
	.ops    = &device_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = LPC_DEVID_ES2,
};
