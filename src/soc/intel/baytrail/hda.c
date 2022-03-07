/* SPDX-License-Identifier: GPL-2.0-only */
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <reg_script.h>

#include <soc/intel/common/hda_verb.h>
#include <soc/iomap.h>
#include <soc/iosf.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>

static const struct reg_script init_ops[] = {
	/* Enable no snoop traffic. */
	REG_PCI_OR16(0x78, 1 << 11),
	/* Configure HDMI codec connection. */
	REG_PCI_OR32(0xc4, 1 << 1),
	REG_PCI_OR8(0x43, (1 << 3) | (1 << 6)),
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_PWRGT_CONTROL, 0xc0),
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_PWRGT_CONTROL, 0x00),
	/* Configure internal settings. */
	REG_PCI_OR32(0xc0, 0x7 << 21),
	REG_PCI_OR32(0xc4, (0x3 << 26) | (1 << 13) | (1 << 10)),
	REG_PCI_WRITE32(0xc8, 0x82a30000),
	REG_PCI_RMW32(0xd0, ~(1 << 31), 0x0),
	/* Disable docking. */
	REG_PCI_RMW8(0x4d, ~(1 << 7), 0),
	REG_SCRIPT_END,
};

static const uint32_t hdmi_codec_verb_table[] = {
	/* coreboot specific header */
	0x80862882, /* vid did for hdmi codec */
	0x00000000, /* subsystem id */
	0x00000003, /* number of jacks */

	/* pin widget 5 - port B */
	0x20471c10,
	0x20471d00,
	0x20471e56,
	0x20471f18,

	/* pin widget 6 - port C */
	0x20571c20,
	0x20571d00,
	0x20571e56,
	0x20571f18,

	/* pin widget 7 - port D */
	0x20671c30,
	0x20671d00,
	0x20671e56,
	0x20671f58,
};

static void hda_init(struct device *dev)
{
	struct resource *res;
	int codec_mask;
	int i;
	u8 *base;

	reg_script_run_on_dev(dev, init_ops);

	res = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (res == NULL)
		return;

	base = res2mmio(res, 0, 0);
	codec_mask = hda_codec_detect(base);

	printk(BIOS_DEBUG, "codec mask = %x\n", codec_mask);
	if (!codec_mask)
		return;

	for (i = 3; i >= 0; i--) {
		if (!((1 << i) & codec_mask))
			continue;
		hda_codec_init(base, i, sizeof(hdmi_codec_verb_table),
				hdmi_codec_verb_table);
	}
}

static const struct device_operations device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= hda_init,
	.ops_pci		= &soc_pci_ops,
};

static const struct pci_driver southcluster __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VID_INTEL,
	.device		= HDA_DEVID,
};
