/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/azalia_device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

static void hda_init(struct device *dev)
{
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_HDA_VERB))
		azalia_audio_init(dev);
}

static struct device_operations hda_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= hda_init,
	.ops_pci		= &pci_dev_ops_pci,
	.scan_bus		= scan_static_bus
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_MTL_AUDIO_1,
	PCI_DID_INTEL_MTL_AUDIO_2,
	PCI_DID_INTEL_MTL_AUDIO_3,
	PCI_DID_INTEL_MTL_AUDIO_4,
	PCI_DID_INTEL_MTL_AUDIO_5,
	PCI_DID_INTEL_MTL_AUDIO_6,
	PCI_DID_INTEL_MTL_AUDIO_7,
	PCI_DID_INTEL_MTL_AUDIO_8,
	PCI_DID_INTEL_SKL_AUDIO,
	PCI_DID_INTEL_SKL_H_AUDIO,
	PCI_DID_INTEL_LWB_AUDIO,
	PCI_DID_INTEL_LWB_AUDIO_SUPER,
	PCI_DID_INTEL_KBL_AUDIO,
	PCI_DID_INTEL_CNL_AUDIO,
	PCI_DID_INTEL_CNP_H_AUDIO,
	PCI_DID_INTEL_ICL_AUDIO,
	PCI_DID_INTEL_CMP_AUDIO,
	PCI_DID_INTEL_CMP_H_AUDIO,
	PCI_DID_INTEL_BSW_AUDIO,
	PCI_DID_INTEL_TGL_AUDIO,
	PCI_DID_INTEL_TGL_H_AUDIO,
	PCI_DID_INTEL_MCC_AUDIO,
	PCI_DID_INTEL_JSP_AUDIO,
	PCI_DID_INTEL_ADP_P_AUDIO,
	PCI_DID_INTEL_ADP_S_AUDIO_1,
	PCI_DID_INTEL_ADP_S_AUDIO_2,
	PCI_DID_INTEL_ADP_S_AUDIO_3,
	PCI_DID_INTEL_ADP_S_AUDIO_4,
	PCI_DID_INTEL_ADP_S_AUDIO_5,
	PCI_DID_INTEL_ADP_S_AUDIO_6,
	PCI_DID_INTEL_ADP_S_AUDIO_7,
	PCI_DID_INTEL_ADP_S_AUDIO_8,
	PCI_DID_INTEL_ADP_M_N_AUDIO_1,
	PCI_DID_INTEL_ADP_M_N_AUDIO_2,
	PCI_DID_INTEL_ADP_M_N_AUDIO_3,
	PCI_DID_INTEL_ADP_M_N_AUDIO_4,
	PCI_DID_INTEL_ADP_M_N_AUDIO_5,
	PCI_DID_INTEL_ADP_M_N_AUDIO_6,
	PCI_DID_INTEL_ADP_M_N_AUDIO_7,
	0
};

static const struct pci_driver pch_hda __pci_driver = {
	.ops		= &hda_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= pci_device_ids,
};
