/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>
#include <soc/acpi.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>

static const char *pcie_device_get_acpi_name(const struct device *dev)
{
	/*
	 * dev->name cannot be assigned in this routine with const struct device *,
	 * instead, it is set up in pcie_device_set_acpi_name ahead of time.
	 */
	assert(dev->name);
	return dev->name;
}

static void soc_pciexp_scan_bridge(struct device *dev)
{
	if (CONFIG(PCIEXP_HOTPLUG) && pciexp_dev_is_slot_hot_plug_cap(dev))
		pciexp_hotplug_scan_bridge(dev);
	else
		pciexp_scan_bridge(dev);
}

static void pcie_device_set_acpi_name(struct device *dev, const char *prefix)
{
	assert(prefix != NULL && strlen(prefix) == 2);
	if (!prefix || strlen(prefix) != 2)
		return;

	char *name = xmalloc(ACPI_NAME_BUFFER_SIZE);
	uint8_t slot = PCI_SLOT(dev->path.pci.devfn);
	uint8_t func = PCI_FUNC(dev->path.pci.devfn);
	snprintf(name, ACPI_NAME_BUFFER_SIZE, "%s%02X", prefix, PCI_DEVFN(slot, func));
	dev->name = name;
}

static void pcie_root_port_init(struct device *rp)
{
	if (!is_pci_bridge(rp))
		return;

	pcie_device_set_acpi_name(rp, "RP");

	struct device *dev = NULL;
	while ((dev = dev_bus_each_child(rp->downstream, dev))) {
		if (!is_pci(dev))
			continue;
		pcie_device_set_acpi_name(dev, "DC");
	}

	pci_dev_init(rp);
}

static const unsigned short pcie_root_port_ids[] = {
	/*
	 * Refer to https://pcisig.com/developers/integrators-list?field_il_comp_product_type_value=All&keys=Intel
	 */
	0x352a,
	0x352b,
	0x352c,
	0x352d,
	0x347a,
	0x0db0,
	0x0db1,
	0x0db2,
	0x0db3,
	0x0db6,
	0x0db7,
	0x0db8,
	0x0db9,
	0
};

static struct device_operations pcie_root_port_ops = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.scan_bus         = soc_pciexp_scan_bridge,
	.reset_bus        = pci_bus_reset,
	.init             = pcie_root_port_init,
	.ops_pci          = &soc_pci_ops,
	.acpi_fill_ssdt   = &acpigen_write_pci_root_port,
	.acpi_name        = &pcie_device_get_acpi_name,
};

static const struct pci_driver pcie_root_port_driver __pci_driver = {
	.ops = &pcie_root_port_ops,
	.vendor = PCI_VID_INTEL,
	.devices = pcie_root_port_ids,
};
