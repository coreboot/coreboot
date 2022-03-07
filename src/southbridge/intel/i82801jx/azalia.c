/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/mmio.h>
#include <device/azalia_device.h>
#include "chip.h"
#include "i82801jx.h"

static int codec_detect(u8 *base)
{
	u32 reg32;

	if (azalia_enter_reset(base) < 0)
		goto no_codec;

	if (azalia_exit_reset(base) < 0)
		goto no_codec;

	/* Read in Codec location (BAR + 0xe)[2..0] */
	reg32 = read32(base + HDA_STATESTS_REG);
	reg32 &= 0x0f;
	if (!reg32)
		goto no_codec;

	return reg32;

no_codec:
	/* Codec not found, put HDA back in reset */
	azalia_enter_reset(base);
	printk(BIOS_DEBUG, "Azalia: No codec!\n");
	return 0;
}

static void azalia_init(struct device *dev)
{
	u8 *base;
	struct resource *res;
	u32 codec_mask;

	// ESD
	pci_update_config32(dev, 0x134, ~0x00ff0000, 2 << 16);

	// Link1 description
	pci_update_config32(dev, 0x140, ~0x00ff0000, 2 << 16);

	// Port VC0 Resource Control Register
	pci_update_config32(dev, 0x114, ~0x000000ff, 1);

	// VCi traffic class
	pci_or_config8(dev, 0x44, 7 << 0); // TC7

	// VCi Resource Control
	pci_or_config32(dev, 0x120, (1 << 31) | (1 << 24) | (0x80 << 0)); /* VCi ID and map */

	/* Set Bus Master */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	// Docking not supported
	pci_and_config8(dev, 0x4d, (u8)~(1 << 7)); // Docking Status

	/* Lock some R/WO bits by writing their current value. */
	pci_update_config32(dev, 0x74, ~0, 0);

	res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!res)
		return;

	// NOTE this will break as soon as the Azalia gets a bar above 4G.
	// Is there anything we can do about it?
	base = res2mmio(res, 0, 0);
	printk(BIOS_DEBUG, "Azalia: base = %p\n", base);
	codec_mask = codec_detect(base);

	if (codec_mask) {
		printk(BIOS_DEBUG, "Azalia: codec_mask = %02x\n", codec_mask);
		azalia_codecs_init(base, codec_mask);
	}
}

static struct device_operations azalia_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= azalia_init,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	0x3a3e,
	0x3a6e,
	0
};

static const struct pci_driver i82801jx_azalia __pci_driver = {
	.ops	= &azalia_ops,
	.vendor	= PCI_VID_INTEL,
	.devices	= pci_device_ids,
};
