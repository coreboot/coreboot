#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "esb6300.h"

static void ac97_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	/* Write the subsystem vendor and device id */
	pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = ac97_set_subsystem,
};
static struct device_operations ac97_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = 0,
	.scan_bus         = 0,
	.enable           = esb6300_enable,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver ac97_audio_driver __pci_driver = {
	.ops    = &ac97_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_6300ESB_AC97_AUDIO,
};
static const struct pci_driver ac97_modem_driver __pci_driver = {
	.ops    = &ac97_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_6300ESB_AC97_MODEM,
};
