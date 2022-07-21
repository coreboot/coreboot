/* SPDX-License-Identifier: GPL-2.0-only */
#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#if CONFIG(HAVE_ACPI_TABLES)
static void ufs_fill_ssdt(const struct device *dev)
{
	struct acpi_dp *dsd;

	const char *scope = acpi_device_scope(dev);

	acpigen_write_scope(scope);
	acpigen_write_device("UFS");
	acpigen_write_ADR_pci_device(dev);
	acpigen_write_name_string("_DDN", "UFS Controller");

	dsd = acpi_dp_new_table("_DSD");
	acpi_dp_add_integer(dsd, "ref-clk-freq", CONFIG_SOC_INTEL_UFS_CLK_FREQ_HZ);
	acpi_dp_write(dsd);
	acpigen_write_device_end();
	acpigen_pop_len();
}
#endif

static struct device_operations dev_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt		= ufs_fill_ssdt,
#endif
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_ADP_UFS,
	0
};

static const struct pci_driver pch_ufs __pci_driver = {
	.ops		= &dev_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= pci_device_ids
};
