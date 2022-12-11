/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "chip.h"

static void ish_fill_ssdt_generator(const struct device *dev)
{
	struct drivers_intel_ish_config *config = dev->chip_info;
	struct device *root = dev->bus->dev;
	struct acpi_dp *dsd;

	if (!config)
		return;

	acpigen_write_scope(acpi_device_path(root));

	dsd = acpi_dp_new_table("_DSD");

	if (config->firmware_name) {
		acpi_dp_add_string(dsd, "firmware-name", config->firmware_name);
		printk(BIOS_INFO, "%s: Set firmware-name: %s\n",
		       acpi_device_path(root), config->firmware_name);
	}

	if (config->add_acpi_dma_property)
		acpi_device_add_dma_property(dsd);

	acpi_dp_write(dsd);

	acpigen_pop_len();	/* Scope */
}

static struct device_operations intel_ish_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.acpi_fill_ssdt		= ish_fill_ssdt_generator,
};

static void intel_ish_enable(struct device *dev)
{
	/* This dev is a generic device that is a child to the ISH PCI device */
	dev->ops = &intel_ish_ops;
}

/* Copy of default_pci_ops_dev with scan_bus addition */
static const struct device_operations pci_ish_device_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = pci_dev_init,
	.scan_bus         = &scan_generic_bus, /* Non-default */
	.ops_pci          = &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_MTL_ISHB,
	PCI_DID_INTEL_CNL_ISHB,
	PCI_DID_INTEL_CML_ISHB,
	PCI_DID_INTEL_TGL_ISHB,
	PCI_DID_INTEL_TGL_H_ISHB,
	PCI_DID_INTEL_ADL_N_ISHB,
	0
};

static const struct pci_driver ish_intel_driver __pci_driver = {
	.ops		= &pci_ish_device_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= pci_device_ids,
};

struct chip_operations drivers_intel_ish_ops = {
	CHIP_NAME("Intel ISH Chip")
	.enable_dev	= intel_ish_enable,
};
