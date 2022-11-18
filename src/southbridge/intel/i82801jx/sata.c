/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <option.h>
#include <types.h>

#include "chip.h"
#include "i82801jx.h"

typedef struct southbridge_intel_i82801jx_config config_t;

static void sata_enable_ahci_mmap(struct device *const dev, const u8 port_map)
{
	int i;
	u32 reg32;
	struct resource *res;

	/* Initialize AHCI memory-mapped space */
	res = probe_resource(dev, PCI_BASE_ADDRESS_5);
	if (!res)
		return;

	u8 *abar = res2mmio(res, 0, 0);
	printk(BIOS_DEBUG, "ABAR: %p\n", abar);

	/* Set AHCI access mode.
	   No other ABAR registers should be accessed before this. */
	reg32 = read32(abar + 0x04);
	reg32 |= 1 << 31;
	write32(abar + 0x04, reg32);

	/* CAP (HBA Capabilities) : enable power management */
	reg32 = read32(abar + 0x00);
	/* CCCS must be set. */
	reg32 |= 0x0c006080;  /* set CCCS+PSC+SSC+SALP+SSS */
	reg32 &= ~0x00020060; /* clear SXS+EMS+PMS */
	write32(abar + 0x00, reg32);

	/* PI (Ports implemented) */
	write32(abar + 0x0c, port_map);
	/* PCH code reads back twice, do we need it, too? */
	(void)read32(abar + 0x0c); /* Read back 1 */
	(void)read32(abar + 0x0c); /* Read back 2 */

	/* VSP (Vendor Specific Register) */
	reg32 = read32(abar + 0xa0);
	reg32 &= ~0x00000001; /* clear SLPD */
	write32(abar + 0xa0, reg32);

	/* Lock R/WO bits in Port command registers. */
	for (i = 0; i < 6; ++i) {
		u8 *addr = abar + 0x118 + (i * 0x80);
		write32(addr, read32(addr));
	}
}

static void sata_program_indexed(struct device *const dev)
{
	u32 reg32;

	pci_write_config8(dev, D31F2_SIDX, 0x18);
	reg32 = pci_read_config32(dev, D31F2_SDAT);
	reg32 &= ~((7 << 6) | (7 << 3) | (7 << 0));
	reg32 |= (3 << 3) | (3 << 0);
	pci_write_config32(dev, D31F2_SDAT, reg32);

	pci_write_config8(dev, D31F2_SIDX, 0x28);
	pci_write_config32(dev, D31F2_SDAT, 0x00cc2080);

	pci_write_config8(dev, D31F2_SIDX, 0x40);
	pci_write_config8(dev, D31F2_SDAT + 2, 0x22);

	pci_write_config8(dev, D31F2_SIDX, 0x78);
	pci_write_config8(dev, D31F2_SDAT + 2, 0x22);

	pci_write_config8(dev, D31F2_SIDX, 0x84);
	reg32 = pci_read_config32(dev, D31F2_SDAT);
	reg32 &= ~((7 << 3) | (7 << 0));
	reg32 |= (3 << 3) | (3 << 0);
	pci_write_config32(dev, D31F2_SDAT, reg32);

	pci_write_config8(dev, D31F2_SIDX, 0x88);
	reg32 = pci_read_config32(dev, D31F2_SDAT);
	reg32 &= ~((7 << 27) | (7 << 24) | (7 << 11) | (7 << 8));
	reg32 &= ~((7 << 19) | (7 << 16) | (7 <<  3) | (7 << 0));
	reg32 |= (4 << 27) | (4 << 24) | (2 << 11) | (2 << 8);
	reg32 |= (4 << 19) | (4 << 16) | (2 <<  3) | (2 << 0);
	pci_write_config32(dev, D31F2_SDAT, reg32);

	pci_write_config8(dev, D31F2_SIDX, 0x8c);
	reg32 = pci_read_config32(dev, D31F2_SDAT);
	reg32 &= ~((7 << 27) | (7 << 24));
	reg32 &= ~((7 << 19) | (7 << 16) | 0xffff);
	reg32 |= (2 << 27) | (2 << 24);
	reg32 |= (2 << 19) | (2 << 16) | 0x00aa;
	pci_write_config32(dev, D31F2_SDAT, reg32);

	pci_write_config8(dev, D31F2_SIDX, 0x94);
	pci_write_config32(dev, D31F2_SDAT, 0x00000022);

	pci_write_config8(dev, D31F2_SIDX, 0xa0);
	reg32 = pci_read_config32(dev, D31F2_SDAT);
	reg32 &= ~((7 << 3) | (7 << 0));
	reg32 |= (3 << 3) | (3 << 0);
	pci_write_config32(dev, D31F2_SDAT, reg32);

	pci_write_config8(dev, D31F2_SIDX, 0xa8);
	reg32 = pci_read_config32(dev, D31F2_SDAT);
	reg32 &= ~((7 << 19) | (7 << 16) | (7 << 3) | (7 << 0));
	reg32 |= (4 << 19) | (4 << 16) | (2 << 3) | (2 << 0);
	pci_write_config32(dev, D31F2_SDAT, reg32);

	pci_write_config8(dev, D31F2_SIDX, 0xac);
	reg32 = pci_read_config32(dev, D31F2_SDAT);
	reg32 &= ~((7 << 19) | (7 << 16) | 0xffff);
	reg32 |= (2 << 19) | (2 << 16) | 0x000a;
	pci_write_config32(dev, D31F2_SDAT, reg32);
}

static void sata_init(struct device *const dev)
{
	u16 reg16;

	/* Get the chip configuration */
	const config_t *const config = dev->chip_info;

	printk(BIOS_DEBUG, "i82801jx_sata: initializing...\n");

	if (config == NULL) {
		printk(BIOS_ERR, "i82801jx_sata: error: "
				 "device not in devicetree.cb!\n");
		return;
	}

	/* Default to AHCI */
	u8 sata_mode = get_uint_option("sata_mode", 0);

	/*
	 * TODO: In contrast to ICH7 and PCH code we don't set
	 * timings, dma and IDE-I/O settings here. Looks like they
	 * became obsolete with the fading of real IDE ports.
	 * Maybe we can safely remove those settings from PCH code and
	 * even ICH7 code if it doesn't use the feature to combine the
	 * IDE and SATA controllers.
	 */

	pci_write_config16(dev, PCI_COMMAND,
			PCI_COMMAND_MASTER |
			PCI_COMMAND_MEMORY | /* read-only in IDE modes */
			PCI_COMMAND_IO);
	if (sata_mode != 0)
		/* No AHCI: clear AHCI base */
		pci_write_config32(dev, PCI_BASE_ADDRESS_5, 0x00000000);

	if (sata_mode == 0) {
		printk(BIOS_DEBUG, "SATA controller in AHCI mode.\n");
	} else {
		printk(BIOS_DEBUG, "SATA controller in native mode.\n");

		/* Enable native mode on both primary and secondary. */
		pci_write_config8(dev, PCI_CLASS_PROG, 0x8f);
	}

	/* Looks like we should only enable decoding here. */
	pci_write_config16(dev, D31F2_IDE_TIM_PRI, (1 << 15));
	pci_write_config16(dev, D31F2_IDE_TIM_SEC, (1 << 15));

	/* Port enable. For AHCI, it's managed in memory mapped space. */
	reg16 = pci_read_config16(dev, 0x92);
	reg16 &= ~0x3f;
	reg16 |= (1 << 15) | ((sata_mode == 0) ? 0x3f : config->sata_port_map);
	pci_write_config16(dev, 0x92, reg16);

	/* SATA clock settings */
	u32 sclkcg = 0;
	if (config->sata_clock_request &&
			!(inb(DEFAULT_GPIOBASE + 0x30) & (1 << (35 - 32))))
		sclkcg |= 1 << 30; /* Enable SATA clock request. */
	/* Disable unused ports. */
	sclkcg |= ((~config->sata_port_map) & 0x3f) << 24;
	/* Must be programmed. */
	sclkcg |= 0x193;
	pci_write_config32(dev, 0x94, sclkcg);

	if (sata_mode == 0)
		sata_enable_ahci_mmap(dev, config->sata_port_map);

	sata_program_indexed(dev);
}

static void sata_enable(struct device *dev)
{
	/* Get the chip configuration */
	const config_t *const config = dev->chip_info;

	u16 map = 0;

	if (!config)
		return;

	/* Default to AHCI */
	u8 sata_mode = get_uint_option("sata_mode", 0);

	/*
	 * Set SATA controller mode early so the resource allocator can
	 * properly assign IO/Memory resources for the controller.
	 */
	if (sata_mode == 0)
		map = 0x0040 | 0x0020; /* SATA mode + all ports on D31:F2 */

	map |= (config->sata_port_map ^ 0x3f) << 8;

	pci_write_config16(dev, 0x90, map);
}

static struct device_operations sata_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= sata_init,
	.enable			= sata_enable,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	0x3a00,
	0x3a02,
	0x3a05,
	0x3a06,
	0x3a20,
	0x3a22,
	0x3a25,
	0x3a26,
	0,
};

static const struct pci_driver pch_sata __pci_driver = {
	.ops	 = &sata_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
