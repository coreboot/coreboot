/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <soc/acpi.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/intel/broadwell/chip.h>

static struct device_operations pci_domain_ops = {
	.read_resources    = &pci_domain_read_resources,
	.set_resources     = &pci_domain_set_resources,
	.scan_bus          = &pci_domain_scan_bus,
#if CONFIG(HAVE_ACPI_TABLES)
	.write_acpi_tables = &northbridge_write_acpi_tables,
#endif
};

static struct device_operations cpu_bus_ops = {
	.read_resources   = noop_read_resources,
	.set_resources    = noop_set_resources,
	.init             = &broadwell_init_cpus,
};

static void broadwell_enable(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	} else if (dev->path.type == DEVICE_PATH_PCI) {
		/* Handle PCH device enable */
		if (PCI_SLOT(dev->path.pci.devfn) > SA_DEV_SLOT_MINIHD &&
		    (dev->ops == NULL || dev->ops->enable == NULL)) {
			broadwell_pch_enable_dev(dev);
		}
	}
}

struct chip_operations soc_intel_broadwell_ops = {
	CHIP_NAME("Intel Broadwell")
	.enable_dev = &broadwell_enable,
	.init       = &broadwell_init_pre_device,
};
