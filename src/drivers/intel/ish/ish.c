/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/cse.h>
#include "chip.h"

static void ish_fill_ssdt_generator(const struct device *dev)
{
	struct drivers_intel_ish_config *config = dev->chip_info;
	struct device *root = dev->upstream->dev;
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

static void intel_ish_get_version(void)
{
	if (CONFIG(SOC_INTEL_CSE_LITE_SYNC_BY_PAYLOAD))
		return;

	struct cse_specific_info *info = cbmem_find(CBMEM_ID_CSE_INFO);
	if (info == NULL)
		return;

	printk(BIOS_DEBUG, "ISH version: %d.%d.%d.%d\n",
		info->cse_fwp_version.ish_partition_info.cur_ish_fw_version.major,
		info->cse_fwp_version.ish_partition_info.cur_ish_fw_version.minor,
		info->cse_fwp_version.ish_partition_info.cur_ish_fw_version.hotfix,
		info->cse_fwp_version.ish_partition_info.cur_ish_fw_version.build);
}

static void intel_ish_final(struct device *dev)
{
	if (CONFIG(DRIVER_INTEL_ISH_HAS_MAIN_FW))
		return;

	if (CONFIG(SOC_INTEL_STORE_ISH_FW_VERSION))
		intel_ish_get_version();
}

/* Copy of default_pci_ops_dev with scan_bus addition */
static const struct device_operations pci_ish_device_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = pci_dev_init,
	.scan_bus         = &scan_generic_bus, /* Non-default */
	.ops_pci          = &pci_dev_ops_pci,
	.final            = intel_ish_final,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_WCL_ISHB,
	PCI_DID_INTEL_PTL_H_ISHB,
	PCI_DID_INTEL_PTL_U_H_ISHB,
	PCI_DID_INTEL_LNL_ISHB,
	PCI_DID_INTEL_MTL_ISHB,
	PCI_DID_INTEL_ARL_ISHB,
	PCI_DID_INTEL_CNL_ISHB,
	PCI_DID_INTEL_CML_ISHB,
	PCI_DID_INTEL_TGL_ISHB,
	PCI_DID_INTEL_TGL_H_ISHB,
	PCI_DID_INTEL_ADL_N_ISHB,
	PCI_DID_INTEL_ADL_P_ISHB,
	0
};

static const struct pci_driver ish_intel_driver __pci_driver = {
	.ops		= &pci_ish_device_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= pci_device_ids,
};

struct chip_operations drivers_intel_ish_ops = {
	.name = "Intel ISH Chip",
	.enable_dev	= intel_ish_enable,
};
