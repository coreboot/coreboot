/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <option.h>
#include <acpi/acpi_sata.h>
#include <types.h>

#include "chip.h"
#include "pch.h"

static inline u32 sir_read(struct device *dev, int idx)
{
	pci_write_config32(dev, SATA_SIRI, idx);
	return pci_read_config32(dev, SATA_SIRD);
}

static inline void sir_write(struct device *dev, int idx, u32 value)
{
	pci_write_config32(dev, SATA_SIRI, idx);
	pci_write_config32(dev, SATA_SIRD, value);
}

static void sata_init(struct device *dev)
{
	u32 reg32;
	u16 reg16;
	/* Get the chip configuration */
	const struct southbridge_intel_ibexpeak_config *config = dev->chip_info;

	printk(BIOS_DEBUG, "SATA: Initializing...\n");

	if (config == NULL) {
		printk(BIOS_ERR, "SATA: ERROR: Device not in devicetree.cb!\n");
		return;
	}

	/* Default to AHCI */
	u8 sata_mode = get_uint_option("sata_mode", 0);

	/* SATA configuration */

	/* Enable BARs */
	pci_write_config16(dev, PCI_COMMAND,
			   PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO);

	if (sata_mode == 0) {
		/* AHCI */
		u32 *abar;

		printk(BIOS_DEBUG, "SATA: Controller in AHCI mode.\n");

		/* Set Interrupt Line */
		/* Interrupt Pin is set by D31IP.PIP */
		pci_write_config8(dev, INTR_LN, 0x0b);

		/* Set timings */
		pci_write_config16(dev, IDE_TIM_PRI, IDE_DECODE_ENABLE);
		pci_write_config16(dev, IDE_TIM_SEC, IDE_DECODE_ENABLE);

		/* for AHCI, Port Enable is managed in memory mapped space */
		reg16 = pci_read_config16(dev, 0x92);
		reg16 &= ~0x3f;	/* 6 ports SKU + ORM */
		reg16 |= 0x8100 | config->sata_port_map;
		pci_write_config16(dev, 0x92, reg16);

		/* SATA Initialization register */
		pci_write_config32(dev, 0x94,
				   ((config->
				     sata_port_map ^ 0x3f) << 24) | 0x183 |
				   0x40000000);
		pci_write_config32(dev, 0x98, 0x00590200);

		/* Initialize AHCI memory-mapped space */
		abar = (u32 *)(uintptr_t)pci_read_config32(dev, PCI_BASE_ADDRESS_5);
		printk(BIOS_DEBUG, "ABAR: %p\n", abar);
		/* CAP (HBA Capabilities) : enable power management */
		reg32 = read32(abar + 0x00);
		reg32 |= 0x0c006000;	// set PSC+SSC+SALP+SSS
		reg32 &= ~0x00020060;	// clear SXS+EMS+PMS
		/* Set ISS, if available */
		if (config->sata_interface_speed_support) {
			reg32 &= ~0x00f00000;
			reg32 |= (config->sata_interface_speed_support & 0x03)
			    << 20;
		}
		write32(abar + 0x00, reg32);
		/* PI (Ports implemented) */
		write32(abar + 0x03, config->sata_port_map);
		(void)read32(abar + 0x03);	/* Read back 1 */
		(void)read32(abar + 0x03);	/* Read back 2 */
		/* CAP2 (HBA Capabilities Extended) */
		reg32 = read32(abar + 0x09);
		reg32 &= ~0x00000002;
		write32(abar + 0x09, reg32);
		/* VSP (Vendor Specific Register */
		reg32 = read32(abar + 0x28);
		reg32 &= ~0x00000005;
		write32(abar + 0x28, reg32);
	} else {
		/* IDE */
		printk(BIOS_DEBUG, "SATA: Controller in plain mode.\n");

		/* No AHCI: clear AHCI base */
		pci_write_config32(dev, 0x24, 0x00000000);

		/* And without AHCI BAR no memory decoding */
		reg16 = pci_read_config16(dev, PCI_COMMAND);
		reg16 &= ~PCI_COMMAND_MEMORY;
		pci_write_config16(dev, PCI_COMMAND, reg16);

		/* Native mode capable on both primary and secondary (0xa)
		 * or'ed with enabled (0x50) = 0xf
		 */
		pci_write_config8(dev, 0x09, 0x8f);

		/* Set Interrupt Line */
		/* Interrupt Pin is set by D31IP.PIP */
		pci_write_config8(dev, INTR_LN, 0xff);

		/* Set timings */
		pci_write_config16(dev, IDE_TIM_PRI, IDE_DECODE_ENABLE);
		pci_write_config16(dev, IDE_TIM_SEC, IDE_DECODE_ENABLE);

		/* Port enable */
		reg16 = pci_read_config16(dev, 0x92);
		reg16 &= ~0x3f;
		reg16 |= config->sata_port_map;
		pci_write_config16(dev, 0x92, reg16);

		/* SATA Initialization register */
		pci_write_config32(dev, 0x94,
				   ((config->
				     sata_port_map ^ 0x3f) << 24) | 0x183);
	}

	/* Additional Programming Requirements */
	sir_write(dev, 0x04, 0x00000000);
	sir_write(dev, 0x28, 0x0a000033);
	reg32 = sir_read(dev, 0x54);
	reg32 &= 0xff000000;
	reg32 |= 0x555555;
	sir_write(dev, 0x54, reg32);
	sir_write(dev, 0x64, 0xcccccccc);
	reg32 = sir_read(dev, 0x68);
	reg32 &= 0xffff0000;
	reg32 |= 0xcccc;
	sir_write(dev, 0x68, reg32);
	reg32 = sir_read(dev, 0x78);
	reg32 &= 0x0000ffff;
	reg32 |= 0x88880000;
	sir_write(dev, 0x78, reg32);
	sir_write(dev, 0x84, 0x001c7000);
	sir_write(dev, 0x88, 0x88888888);
	sir_write(dev, 0xa0, 0x001c7000);
	// a4
	sir_write(dev, 0xc4, 0x0c0c0c0c);
	sir_write(dev, 0xc8, 0x0c0c0c0c);
	sir_write(dev, 0xd4, 0x10000000);
}

static void sata_enable(struct device *dev)
{
	/* Get the chip configuration */
	const struct southbridge_intel_ibexpeak_config *config = dev->chip_info;
	u16 map = 0;

	if (!config)
		return;

	u8 sata_mode = get_uint_option("sata_mode", 0);

	/*
	 * Set SATA controller mode early so the resource allocator can
	 * properly assign IO/Memory resources for the controller.
	 */
	if (sata_mode == 0)
		map = 0x0060;

	map |= (config->sata_port_map ^ 0x3f) << 8;

	pci_write_config16(dev, 0x90, map);
}

static void sata_fill_ssdt(const struct device *dev)
{
	const struct southbridge_intel_ibexpeak_config *config = dev->chip_info;
	generate_sata_ssdt_ports("\\_SB_.PCI0.SATA", config->sata_port_map);
}

static struct device_operations sata_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = sata_init,
	.enable = sata_enable,
	.acpi_fill_ssdt = sata_fill_ssdt,
	.ops_pci = &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_IBEXPEAK_MOBILE_SATA_IDE_1,
	PCI_DID_INTEL_IBEXPEAK_MOBILE_SATA_AHCI,
	PCI_DID_INTEL_IBEXPEAK_MOBILE_SATA_IDE_2,
	0
};

static const struct pci_driver pch_sata __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
