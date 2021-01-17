/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/azalia_device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/intel/common/hda_verb.h>
#include <soc/ramstage.h>

#if CONFIG(SOC_INTEL_COMMON_BLOCK_HDA_VERB)
static void codecs_init(uint8_t *base, u32 codec_mask)
{
	int i;

	/* Can support up to 4 codecs */
	for (i = 3; i >= 0; i--) {
		if (codec_mask & (1 << i))
			hda_codec_init(base, i,
				       cim_verb_data_size, cim_verb_data);
	}

	if (pc_beep_verbs_size)
		hda_codec_write(base, pc_beep_verbs_size, pc_beep_verbs);
}

static void hda_init(struct device *dev)
{
	struct resource *res;
	int codec_mask;
	uint8_t *base;

	res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (!res)
		return;

	base = res2mmio(res, 0, 0);
	if (!base)
		return;

	codec_mask = hda_codec_detect(base);
	if (codec_mask) {
		printk(BIOS_INFO, "HDA: codec_mask = %02x\n", codec_mask);
		codecs_init(base, codec_mask);
	}
}
#endif

static struct device_operations hda_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
#if CONFIG(SOC_INTEL_COMMON_BLOCK_HDA_VERB)
	.init			= hda_init,
#endif
	.ops_pci		= &pci_dev_ops_pci,
	.scan_bus		= scan_static_bus
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_INTEL_SKL_AUDIO,
	PCI_DEVICE_ID_INTEL_SKL_H_AUDIO,
	PCI_DEVICE_ID_INTEL_LWB_AUDIO,
	PCI_DEVICE_ID_INTEL_LWB_AUDIO_SUPER,
	PCI_DEVICE_ID_INTEL_KBL_AUDIO,
	PCI_DEVICE_ID_INTEL_CNL_AUDIO,
	PCI_DEVICE_ID_INTEL_CNP_H_AUDIO,
	PCI_DEVICE_ID_INTEL_ICL_AUDIO,
	PCI_DEVICE_ID_INTEL_CMP_AUDIO,
	PCI_DEVICE_ID_INTEL_CMP_H_AUDIO,
	PCI_DEVICE_ID_INTEL_BSW_AUDIO,
	PCI_DEVICE_ID_INTEL_TGL_AUDIO,
	PCI_DEVICE_ID_INTEL_MCC_AUDIO,
	PCI_DEVICE_ID_INTEL_JSP_AUDIO,
	PCI_DEVICE_ID_INTEL_ADP_P_AUDIO,
	PCI_DEVICE_ID_INTEL_ADP_S_AUDIO_1,
	PCI_DEVICE_ID_INTEL_ADP_S_AUDIO_2,
	PCI_DEVICE_ID_INTEL_ADP_S_AUDIO_3,
	PCI_DEVICE_ID_INTEL_ADP_S_AUDIO_4,
	PCI_DEVICE_ID_INTEL_ADP_S_AUDIO_5,
	PCI_DEVICE_ID_INTEL_ADP_S_AUDIO_6,
	PCI_DEVICE_ID_INTEL_ADP_S_AUDIO_7,
	PCI_DEVICE_ID_INTEL_ADP_S_AUDIO_8,
	PCI_DEVICE_ID_INTEL_ADP_M_AUDIO_1,
	PCI_DEVICE_ID_INTEL_ADP_M_AUDIO_2,
	PCI_DEVICE_ID_INTEL_ADP_M_AUDIO_3,
	PCI_DEVICE_ID_INTEL_ADP_M_AUDIO_4,
	PCI_DEVICE_ID_INTEL_ADP_M_AUDIO_5,
	PCI_DEVICE_ID_INTEL_ADP_M_AUDIO_6,
	PCI_DEVICE_ID_INTEL_ADP_M_AUDIO_7,
	0
};

static const struct pci_driver pch_hda __pci_driver = {
	.ops		= &hda_ops,
	.vendor		= PCI_VENDOR_ID_INTEL,
	.devices	= pci_device_ids,
};
