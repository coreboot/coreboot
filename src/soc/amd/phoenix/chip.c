/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Update for Phoenix */

#include <amdblocks/data_fabric.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include <types.h>
#include "chip.h"

struct device_operations phoenix_cpu_bus_ops = {
	.read_resources	= noop_read_resources,
	.set_resources	= noop_set_resources,
	.init		= mp_cpu_bus_init,
	.acpi_fill_ssdt	= generate_cpu_entries,
};

static const char *soc_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	printk(BIOS_WARNING, "Unknown PCI device: dev: %d, fn: %d\n",
	       PCI_SLOT(dev->path.pci.devfn), PCI_FUNC(dev->path.pci.devfn));
	return NULL;
};

struct device_operations phoenix_pci_domain_ops = {
	.read_resources	= pci_domain_read_resources,
	.set_resources	= pci_domain_set_resources,
	.scan_bus	= pci_domain_scan_bus,
	.acpi_name	= soc_acpi_name,
};

static void soc_init(void *chip_info)
{
	default_dev_ops_root.write_acpi_tables = agesa_write_acpi_tables;

	fsp_silicon_init();

	data_fabric_set_mmio_np();

	fch_init(chip_info);
}

static void soc_final(void *chip_info)
{
	fch_final(chip_info);
}

struct chip_operations soc_amd_phoenix_ops = {
	CHIP_NAME("AMD Phoenix SoC")
	.init = soc_init,
	.final = soc_final
};
