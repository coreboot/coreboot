/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <amdblocks/hda.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/azalia_device.h>

static const unsigned short pci_device_ids[] = {
	PCI_DID_AMD_SB900_HDA,
	PCI_DID_AMD_CZ_HDA,
	PCI_DID_AMD_FAM17H_HDA1,
	0
};

static const char *hda_acpi_name(const struct device *dev)
{
	return "AZHD";
}

__weak void hda_soc_ssdt_quirks(const struct device *dev)
{
}

static void hda_fill_ssdt(const struct device *dev)
{
	acpi_device_write_pci_dev(dev);
	hda_soc_ssdt_quirks(dev);
}

static struct device_operations hda_audio_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.ops_pci		= &pci_dev_ops_pci,
	.acpi_name		= hda_acpi_name,
	.acpi_fill_ssdt		= hda_fill_ssdt,
};

static const struct pci_driver hdaaudio_driver __pci_driver = {
	.ops			= CONFIG(AZALIA_PLUGIN_SUPPORT) ?
				&default_azalia_audio_ops : &hda_audio_ops,
	.vendor			= PCI_VID_AMD,
	.devices		= pci_device_ids,
};
