/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <acpi/acpi_device.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <intelblocks/tcss.h>
#include <soc/pci_devs.h>

#define INTEL_TBT_IMR_VALID_UUID	"C44D002F-69F9-4E7D-A904-A7BAABDF43F7"
#define INTEL_TBT_WAKE_SUPPORTED_UUID	"6C501103-C189-4296-BA72-9BF5A26EBE5D"

#if CONFIG(HAVE_ACPI_TABLES)
static const char *tbt_dma_acpi_name(const struct device *dev)
{
	switch (dev->path.pci.devfn) {
	case SA_DEVFN_TCSS_DMA0:
		return "TDM0";
	case SA_DEVFN_TCSS_DMA1:
		return "TDM1";
	default:
		return NULL;
	}
}

static void tbt_dma_fill_ssdt(const struct device *dev)
{
	struct acpi_dp *dsd, *pkg;

	if (tcss_ops.valid_tbt_auth && !tcss_ops.valid_tbt_auth())
		return;

	acpigen_write_scope(acpi_device_path(dev));

	dsd = acpi_dp_new_table("_DSD");

	/* Indicate that device has valid IMR. */
	pkg = acpi_dp_new_table(INTEL_TBT_IMR_VALID_UUID);
	acpi_dp_add_integer(pkg, "IMR_VALID", 1);
	acpi_dp_add_package(dsd, pkg);

	/* Indicate that device is wake capable. */
	pkg = acpi_dp_new_table(INTEL_TBT_WAKE_SUPPORTED_UUID);
	acpi_dp_add_integer(pkg, "WAKE_SUPPORTED", 1);

	acpi_dp_add_package(dsd, pkg);
	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Scope */
}
#endif

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_MTL_M_TBT_DMA0,
	PCI_DID_INTEL_MTL_P_TBT_DMA0,
	PCI_DID_INTEL_MTL_P_TBT_DMA1,
	PCI_DID_INTEL_TGL_TBT_DMA0,
	PCI_DID_INTEL_TGL_TBT_DMA1,
	PCI_DID_INTEL_TGL_H_TBT_DMA0,
	PCI_DID_INTEL_TGL_H_TBT_DMA1,
	PCI_DID_INTEL_ADL_TBT_DMA0,
	PCI_DID_INTEL_ADL_TBT_DMA1,
	0
};

static struct device_operations usb4_dev_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.scan_bus		= scan_generic_bus,
	.ops_pci		= &pci_dev_ops_pci,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name		= tbt_dma_acpi_name,
	.acpi_fill_ssdt		= tbt_dma_fill_ssdt,
#endif
};

static const struct pci_driver usb4_driver __pci_driver = {
	.ops			= &usb4_dev_ops,
	.vendor			= PCI_VID_INTEL,
	.devices		= pci_device_ids,
};
