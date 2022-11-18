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

typedef struct southbridge_intel_bd82x6x_config config_t;

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

static void sata_read_resources(struct device *dev)
{
	struct resource *res;

	pci_dev_read_resources(dev);

	/* Assign fixed resources for IDE legacy mode */

	u8 sata_mode = get_uint_option("sata_mode", 0);
	if (sata_mode != 2)
		return;

	res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (res) {
		res->base = 0x1f0;
		res->size = 8;
		res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
	}

	res = probe_resource(dev, PCI_BASE_ADDRESS_1);
	if (res) {
		res->base = 0x3f4;
		res->size = 4;
		res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
	}

	res = probe_resource(dev, PCI_BASE_ADDRESS_2);
	if (res) {
		res->base = 0x170;
		res->size = 8;
		res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
	}

	res = probe_resource(dev, PCI_BASE_ADDRESS_3);
	if (res) {
		res->base = 0x374;
		res->size = 4;
		res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
	}
}

static void sata_set_resources(struct device *dev)
{
	/* work around bug in pci_dev_set_resources(), it bails out on FIXED */
	u8 sata_mode = get_uint_option("sata_mode", 0);
	if (sata_mode == 2) {
		unsigned int i;
		for (i = PCI_BASE_ADDRESS_0; i <= PCI_BASE_ADDRESS_3; i += 4) {
			struct resource *const res = probe_resource(dev, i);
			if (res)
				res->flags &= ~IORESOURCE_FIXED;
		}
	}

	pci_dev_set_resources(dev);
}

static void sata_init(struct device *dev)
{
	u32 reg32;
	u16 reg16;
	/* Get the chip configuration */
	config_t *config = dev->chip_info;

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

	/* AHCI */
	if (sata_mode == 0) {
		u8 *abar;

		printk(BIOS_DEBUG, "SATA: Controller in AHCI mode.\n");

		pci_write_config16(dev, IDE_TIM_PRI, IDE_DECODE_ENABLE);
		pci_write_config16(dev, IDE_TIM_SEC, IDE_DECODE_ENABLE);

		/* for AHCI, Port Enable is managed in memory mapped space */
		reg16 = pci_read_config16(dev, 0x92);
		reg16 &= ~0x3f; /* 6 ports SKU + ORM */
		reg16 |= 0x8000 | config->sata_port_map;
		pci_write_config16(dev, 0x92, reg16);

		/* SATA Initialization register */
		pci_write_config32(dev, 0x94, ((config->sata_port_map ^ 0x3f) << 24) | 0x183);

		/* Initialize AHCI memory-mapped space */
		abar = (u8 *)(uintptr_t)pci_read_config32(dev, PCI_BASE_ADDRESS_5);
		printk(BIOS_DEBUG, "ABAR: %p\n", abar);
		/* CAP (HBA Capabilities) : enable power management */
		reg32 = read32(abar + 0x00);
		reg32 |= 0x0c006000;  // set PSC+SSC+SALP+SSS
		reg32 &= ~0x00020060; // clear SXS+EMS+PMS
		/* Set ISS, if available */
		if (config->sata_interface_speed_support)
		{
			reg32 &= ~0x00f00000;
			reg32 |= (config->sata_interface_speed_support & 0x03)
			  << 20;
		}
		write32(abar + 0x00, reg32);
		/* PI (Ports implemented) */
		write32(abar + 0x0c, config->sata_port_map);
		(void)read32(abar + 0x0c); /* Read back 1 */
		(void)read32(abar + 0x0c); /* Read back 2 */
		/* CAP2 (HBA Capabilities Extended)*/
		reg32 = read32(abar + 0x24);
		reg32 &= ~0x00000002;
		write32(abar + 0x24, reg32);
		/* VSP (Vendor Specific Register */
		reg32 = read32(abar + 0xa0);
		reg32 &= ~0x00000005;
		write32(abar + 0xa0, reg32);
	} else {
	        /* IDE */

		/* Without AHCI BAR no memory decoding */
		pci_and_config16(dev, PCI_COMMAND, ~PCI_COMMAND_MEMORY);

		if (sata_mode == 1) {
			/* Native mode on both primary and secondary. */
			pci_or_config8(dev, 0x09, 0x05);
			printk(BIOS_DEBUG, "SATA: Controller in IDE compat mode.\n");
		} else {
			/* Legacy mode on both primary and secondary. */
			pci_and_config8(dev, 0x09, ~0x05);
			printk(BIOS_DEBUG, "SATA: Controller in IDE legacy mode.\n");
		}

		/* Enable I/O decoding */
		pci_write_config16(dev, IDE_TIM_PRI, IDE_DECODE_ENABLE);
		pci_write_config16(dev, IDE_TIM_SEC, IDE_DECODE_ENABLE);

		/* Port enable + OOB retry mode */
		pci_update_config16(dev, 0x92, ~0x3f, config->sata_port_map | 0x8000);

		/* SATA Initialization register */
		pci_write_config32(dev, 0x94, ((config->sata_port_map ^ 0x3f) << 24) | 0x183);
	}

	/* Set Gen3 Transmitter settings if needed */
	if (config->sata_port0_gen3_tx)
		pch_iobp_update(SATA_IOBP_SP0G3IR, 0,
				config->sata_port0_gen3_tx);

	if (config->sata_port1_gen3_tx)
		pch_iobp_update(SATA_IOBP_SP1G3IR, 0,
				config->sata_port1_gen3_tx);

	/* Additional Programming Requirements */
	sir_write(dev, 0x04, 0x00001600);
	sir_write(dev, 0x28, 0xa0000033);
	reg32 = sir_read(dev, 0x54);
	reg32 &= 0xff000000;
	reg32 |= 0x5555aa;
	sir_write(dev, 0x54, reg32);
	sir_write(dev, 0x64, 0xcccc8484);
	reg32 = sir_read(dev, 0x68);
	reg32 &= 0xffff0000;
	reg32 |= 0xcccc;
	sir_write(dev, 0x68, reg32);
	reg32 = sir_read(dev, 0x78);
	reg32 &= 0x0000ffff;
	reg32 |= 0x88880000;
	sir_write(dev, 0x78, reg32);
	sir_write(dev, 0x84, 0x001c7000);
	sir_write(dev, 0x88, 0x88338822);
	sir_write(dev, 0xa0, 0x001c7000);
	// a4
	sir_write(dev, 0xc4, 0x0c0c0c0c);
	sir_write(dev, 0xc8, 0x0c0c0c0c);
	sir_write(dev, 0xd4, 0x10000000);

	pch_iobp_update(0xea004001, 0x3fffffff, 0xc0000000);
	pch_iobp_update(0xea00408a, 0xfffffcff, 0x00000100);

	pci_update_config32(dev, 0x98,
		~(1 << 16 | 0x3f << 7 | 3 << 5 | 3 << 3),
		1 << 24 | 1 << 22 | 1 << 20 | 1 << 19 |
		1 << 18 | 1 << 14 | 0x04 << 7 | 1 << 3);
}

static void sata_enable(struct device *dev)
{
	/* Get the chip configuration */
	config_t *config = dev->chip_info;
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

static const char *sata_acpi_name(const struct device *dev)
{
	return "SATA";
}

static void sata_fill_ssdt(const struct device *dev)
{
	config_t *config = dev->chip_info;
	generate_sata_ssdt_ports("\\_SB_.PCI0.SATA", config->sata_port_map);
}

static struct device_operations sata_ops = {
	.read_resources		= sata_read_resources,
	.set_resources		= sata_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.acpi_fill_ssdt		= sata_fill_ssdt,
	.init			= sata_init,
	.enable			= sata_enable,
	.ops_pci		= &pci_dev_ops_pci,
	.acpi_name		= sata_acpi_name,
};

static const unsigned short pci_device_ids[] = { 0x1c00, 0x1c01, 0x1c02, 0x1c03,
						 0x1e00, 0x1e01, 0x1e02, 0x1e03,
						 0 };

static const struct pci_driver pch_sata __pci_driver = {
	.ops	 = &sata_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
