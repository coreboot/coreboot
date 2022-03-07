/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/mmio.h>
#include <device/azalia_device.h>

#include "pch.h"
#include "hda_verb.h"

static void azalia_pch_init(struct device *dev, u8 *base)
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

		if (!pch_is_lp()) {
			pci_and_config16(dev, 0x78, ~(1 << 11));
		}
	} else
		printk(BIOS_DEBUG, "Azalia: V1CTL disabled.\n");

	reg32 = pci_read_config32(dev, 0x114);
	reg32 &= ~0xfe;
	pci_write_config32(dev, 0x114, reg32);

	// Set VCi enable bit
	if (pci_read_config32(dev, 0x120) & ((1 << 24) | (1 << 25) | (1 << 26))) {
		reg32 = pci_read_config32(dev, 0x120);
		if (pch_is_lp())
			reg32 &= ~(1 << 31);
		else
			reg32 |= (1 << 31);
		pci_write_config32(dev, 0x120, reg32);
	}

	reg8 = pci_read_config8(dev, 0x43);
	if (pch_is_lp())
		reg8 &= ~(1 << 6);
	else
		reg8 |= (1 << 4);
	pci_write_config8(dev, 0x43, reg8);

	if (!pch_is_lp())
		pci_or_config32(dev, 0xc0, 1 << 17);

	/* Additional programming steps */
	reg32 = pci_read_config32(dev, 0xc4);
	if (pch_is_lp())
		reg32 |= (1 << 24);
	else
		reg32 |= (1 << 14);
	pci_write_config32(dev, 0xc4, reg32);

	if (!pch_is_lp())
		pci_and_config32(dev, 0xd0, ~(1 << 31));

	// Docking not supported
	pci_and_config8(dev, 0x4d, (u8)~(1 << 7)); // Docking Status

	if (pch_is_lp()) {
		reg16 = read32(base + 0x0012);
		reg16 |= (1 << 0);
		write32(base + 0x0012, reg16);

		/* disable Auto Voltage Detector */
		pci_or_config8(dev, 0x42, 1 << 2);
	}
}

static void azalia_init(struct device *dev)
{
	u8 *base;
	struct resource *res;
	u32 codec_mask;

	/* Find base address */
	res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!res)
		return;

	base = res2mmio(res, 0, 0);
	printk(BIOS_DEBUG, "Azalia: base = %p\n", base);

	/* Set Bus Master */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	azalia_pch_init(dev, base);

	codec_mask = hda_codec_detect(base);

	if (codec_mask) {
		printk(BIOS_DEBUG, "Azalia: codec_mask = %02x\n", codec_mask);
		azalia_codecs_init(base, codec_mask);
	}
}

static void azalia_final(struct device *dev)
{
	/* Set HDCFG.BCLD */
	pci_or_config16(dev, 0x40, 1 << 1);
}

static struct device_operations azalia_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= azalia_init,
	.final			= azalia_final,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_LPT_H_AUDIO,
	PCI_DID_INTEL_LPT_LP_AUDIO,
	0
};

static const struct pci_driver pch_azalia __pci_driver = {
	.ops	 = &azalia_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
