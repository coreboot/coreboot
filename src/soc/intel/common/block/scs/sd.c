/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/sd.h>

#if CONFIG(HAVE_ACPI_TABLES)
static void sd_fill_ssdt(const struct device *dev)
{
	const char *path;
	struct acpi_gpio default_gpio = { 0 };
	struct acpi_dp *dp;

	if (sd_fill_soc_gpio_info(&default_gpio, dev) != 0)
		return;

	/* Use device path as the Scope for the SSDT */
	path = acpi_device_path(dev);
	if (!path)
		return;
	acpigen_write_scope(path);
	acpigen_write_name("_CRS");

	/* Write GpioInt() as default (if set) or custom from devicetree */
	acpigen_write_resourcetemplate_header();
	acpi_device_write_gpio(&default_gpio);
	acpigen_write_resourcetemplate_footer();

	/* Bind the cd-gpio name to the GpioInt() resource */
	dp = acpi_dp_new_table("_DSD");
	if (!dp)
		return;
	acpi_dp_add_gpio(dp, "cd-gpio", path, 0, 0, 1);
	acpi_dp_write(dp);

	acpigen_pop_len();
}
#endif

struct device_operations sd_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt		= sd_fill_ssdt,
#endif
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_APL_SD,
	PCI_DID_INTEL_CNL_SD,
	PCI_DID_INTEL_GLK_SD,
	PCI_DID_INTEL_CNP_H_SD,
	PCI_DID_INTEL_CMP_SD,
	PCI_DID_INTEL_CMP_H_SD,
	PCI_DID_INTEL_MCC_SD,
	PCI_DID_INTEL_JSP_SD,
	0
};

static const struct pci_driver pch_sd __pci_driver = {
	.ops	= &sd_ops,
	.vendor	= PCI_VID_INTEL,
	.devices	= pci_device_ids
};
