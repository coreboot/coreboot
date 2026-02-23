/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/azalia_device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/mmio.h>
#include <soc/pch.h>
#include <soc/rcba.h>
#include <southbridge/intel/lynxpoint/hda_verb.h>

static void hda_pch_init(struct device *dev, u8 *base)
{
	u8 reg8;
	u16 reg16;
	u32 reg32;

	if (RCBA32(0x2030) & (1 << 31)) {
		reg32 = pci_read_config32(dev, 0x120);
		reg32 &= 0xf8ffff01;
		reg32 |= (1 << 25);
		reg32 |= RCBA32(0x2030) & 0xfe;
		pci_write_config32(dev, 0x120, reg32);
	} else
		printk(BIOS_DEBUG, "HDA: V1CTL disabled.\n");

	reg32 = pci_read_config32(dev, 0x114);
	reg32 &= ~0xfe;
	pci_write_config32(dev, 0x114, reg32);

	// Set VCi enable bit
	if (pci_read_config32(dev, 0x120) & ((1 << 24) |
					     (1 << 25) | (1 << 26))) {
		reg32 = pci_read_config32(dev, 0x120);
		reg32 &= ~(1 << 31);
		pci_write_config32(dev, 0x120, reg32);
	}

	/* Additional programming steps */
	reg32 = pci_read_config32(dev, 0xc4);
	reg32 |= (1 << 24);
	pci_write_config32(dev, 0xc4, reg32);

	reg8 = pci_read_config8(dev, 0x4d); // Docking Status
	reg8 &= ~(1 << 7); // Docking not supported
	pci_write_config8(dev, 0x4d, reg8);

	reg16 = read32(base + 0x0012);
	reg16 |= (1 << 0);
	write32(base + 0x0012, reg16);

	/* disable Auto Voltage Detector */
	reg8 = pci_read_config8(dev, 0x42);
	reg8 |= (1 << 2);
	pci_write_config8(dev, 0x42, reg8);
}

static void hda_init(struct device *dev)
{
	u8 *base;
	struct resource *res;
	u32 codec_mask;

	/* Find base address */
	res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!res)
		return;

	base = res2mmio(res, 0, 0);
	printk(BIOS_DEBUG, "HDA: base = %p\n", base);

	/* Set Bus Master */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	hda_pch_init(dev, base);

	codec_mask = hda_codec_detect(base);

	if (codec_mask) {
		printk(BIOS_DEBUG, "HDA: codec_mask = %02x\n", codec_mask);
		azalia_codecs_init(base, codec_mask);
	}
}

static void hda_enable(struct device *dev)
{
	u16 reg16;
	u8 reg8;

	reg8 = pci_read_config8(dev, 0x43);
	reg8 |= 0x6f;
	pci_write_config8(dev, 0x43, reg8);

	if (!dev->enabled) {
		/* Route I/O buffers to ADSP function */
		reg8 = pci_read_config8(dev, 0x42);
		reg8 |= (1 << 7) | (1 << 6);
		pci_write_config8(dev, 0x42, reg8);

		printk(BIOS_INFO, "HDA disabled, I/O buffers routed to ADSP\n");

		/* Ensure memory, io, and bus master are all disabled */
		reg16 = pci_read_config16(dev, PCI_COMMAND);
		reg16 &= ~(PCI_COMMAND_MASTER |
			   PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
		pci_write_config16(dev, PCI_COMMAND, reg16);

		/* Disable this device */
		pch_disable_devfn(dev);
	}
}

static void hda_final(struct device *dev)
{
	/* Set HDCFG.BCLD */
	pci_or_config16(dev, 0x40, 1 << 1);
}

static struct device_operations hda_ops = {
	.read_resources		= &pci_dev_read_resources,
	.set_resources		= &pci_dev_set_resources,
	.enable_resources	= &pci_dev_enable_resources,
	.init			= &hda_init,
	.enable			= &hda_enable,
	.final			= &hda_final,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	0x9c20, /* LynxPoint-LP */
	0x9ca0, /* WildcatPoint */
	0
};

static const struct pci_driver pch_hda __pci_driver = {
	.ops	 = &hda_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
