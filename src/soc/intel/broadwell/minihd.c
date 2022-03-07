/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/azalia_device.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/mmio.h>
#include <soc/igd.h>
#include <southbridge/intel/lynxpoint/hda_verb.h>

static const u32 minihd_verb_table[] = {
	/* coreboot specific header */
	0x80862808,	/* Codec Vendor / Device ID: Intel Broadwell Mini-HD */
	0x80860101,	/* Subsystem ID */
	4,		/* Number of jacks */

	/* Enable 3rd Pin and Converter Widget */
	0x00878101,

	/* Pin Widget 5 - PORT B */
	0x00571c10,
	0x00571d00,
	0x00571e56,
	0x00571f18,

	/* Pin Widget 6 - PORT C */
	0x00671c20,
	0x00671d00,
	0x00671e56,
	0x00671f18,

	/* Pin Widget 7 - PORT D */
	0x00771c30,
	0x00771d00,
	0x00771e56,
	0x00771f18,

	/* Disable 3rd Pin and Converter Widget */
	0x00878100,

	/* Dummy entries to fill out the table */
	0x00878100,
	0x00878100,
};

static void minihd_init(struct device *dev)
{
	struct resource *res;
	u32 reg32;
	u8 *base;
	int codec_mask, i;

	/* Find base address */
	res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (!res)
		return;

	base = res2mmio(res, 0, 0);
	printk(BIOS_DEBUG, "Mini-HD: base = %p\n", base);

	/* Set Bus Master */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	/* Mini-HD configuration */
	reg32 = read32(base + 0x100c);
	reg32 &= 0xfffc0000;
	reg32 |= 0x4;
	write32(base + 0x100c, reg32);

	reg32 = read32(base + 0x1010);
	reg32 &= 0xfffc0000;
	reg32 |= 0x4b;
	write32(base + 0x1010, reg32);

	/* Init the codec and write the verb table */
	codec_mask = hda_codec_detect(base);

	if (codec_mask) {
		for (i = 3; i >= 0; i--) {
			if (codec_mask & (1 << i))
				azalia_codec_init(base, i, minihd_verb_table,
						  sizeof(minihd_verb_table));
		}
	}

	/* Set EM4/EM5 registers */
	write32(base + 0x0100c, igd_get_reg_em4());
	write32(base + 0x01010, igd_get_reg_em5());
}

static struct device_operations minihd_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= minihd_init,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	0x0a0c, /* Haswell */
	0x160c, /* Broadwell */
	0
};

static const struct pci_driver minihd_driver __pci_driver = {
	.ops	 = &minihd_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
