/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/acpi.h>

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_WCL_DTT,
	PCI_DID_INTEL_PTL_DTT,
	PCI_DID_INTEL_RPL_DTT,
	PCI_DID_INTEL_MTL_DTT,
	PCI_DID_INTEL_ARL_S_DTT,
	PCI_DID_INTEL_CML_DTT,
	PCI_DID_INTEL_TGL_DTT,
	PCI_DID_INTEL_JSL_DTT,
	PCI_DID_INTEL_ADL_DTT,
	0
};

static void dtt_acpi_fill_ssdt(const struct device *dev)
{
	/* Skip if DPTF driver or common DPTF ASL in use since TCPU device will already exist */
	if (CONFIG(DRIVERS_INTEL_DPTF) || CONFIG(SOC_INTEL_COMMON_BLOCK_DTT_STATIC_ASL))
		return;

	const char *scope = acpi_device_scope(dev);

	if (!scope)
		return;

	acpigen_write_scope(scope);
	acpigen_write_device(soc_acpi_name(dev));
	acpigen_write_ADR_pci_device(dev);
	acpigen_write_STA(acpi_device_status(dev));
	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}

static struct device_operations dptf_dev_ops = {
	.read_resources			= pci_dev_read_resources,
	.set_resources			= pci_dev_set_resources,
	.enable_resources		= pci_dev_enable_resources,
	.scan_bus			= scan_generic_bus,
	.ops_pci			= &pci_dev_ops_pci,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt			= dtt_acpi_fill_ssdt,
#endif
};

static const struct pci_driver pch_dptf __pci_driver = {
	.ops				= &dptf_dev_ops,
	.vendor				= PCI_VID_INTEL,
	.devices			= pci_device_ids,
};
