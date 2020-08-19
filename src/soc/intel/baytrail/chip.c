/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <arch/pci_ops.h>

#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include "chip.h"

static struct device_operations pci_domain_ops = {
	.read_resources   = pci_domain_read_resources,
	.set_resources    = pci_domain_set_resources,
	.scan_bus         = pci_domain_scan_bus,
};

static struct device_operations cpu_bus_ops = {
	.read_resources   = noop_read_resources,
	.set_resources    = noop_set_resources,
	.init             = baytrail_init_cpus,
};

static void enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	} else if (dev->path.type == DEVICE_PATH_PCI) {
		/* Handle south cluster enablement. */
		if (PCI_SLOT(dev->path.pci.devfn) > GFX_DEV &&
		    (dev->ops == NULL || dev->ops->enable == NULL)) {
			southcluster_enable_dev(dev);
		}
	}
}

/* Called at BS_DEV_INIT_CHIPS time -- very early. Just after BS_PRE_DEVICE. */
static void soc_init(void *chip_info)
{
	baytrail_init_pre_device(chip_info);
}

struct chip_operations soc_intel_baytrail_ops = {
	CHIP_NAME("Intel BayTrail SoC")
	.enable_dev = enable_dev,
	.init = soc_init,
};

struct pci_operations soc_pci_ops = {
	.set_subsystem = &pci_dev_set_subsystem,
};
