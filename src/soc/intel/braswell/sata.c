/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/sata.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include "chip.h"

typedef struct soc_intel_braswell_config config_t;

static void sata_init(struct device *dev)
{
}

static void sata_enable(struct device *dev)
{
	southcluster_enable_dev(dev);
}

static struct device_operations sata_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= sata_init,
	.enable			= sata_enable,
	.ops_pci		= &soc_pci_ops,
};

static const unsigned short pci_device_ids[] = {
	AHCI1_DEVID,	/* AHCI */
	0,
};

static const struct pci_driver soc_sata __pci_driver = {
	.ops     = &sata_ops,
	.vendor  = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
