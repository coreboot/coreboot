/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Update for Phoenix */

#include <acpi/acpigen_pci.h>
#include <amdblocks/acpi.h>
#include <amdblocks/data_fabric.h>
#include <amdblocks/fsp.h>
#include <amdblocks/root_complex.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/cpu.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include <types.h>
#include <vendorcode/amd/opensil/opensil.h>
#include "chip.h"

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
	.read_resources	= amd_pci_domain_read_resources,
	.set_resources	= pci_domain_set_resources,
	.scan_bus	= amd_pci_domain_scan_bus,
	.init		= amd_pci_domain_init,
	.acpi_name	= soc_acpi_name,
	.acpi_fill_ssdt	= pci_domain_fill_ssdt,
};

static void soc_init(void *chip_info)
{
	default_dev_ops_root.write_acpi_tables = soc_acpi_write_tables;

	if (CONFIG(PLATFORM_USES_FSP2_0)) {
		amd_fsp_silicon_init();
	} else {
		setup_opensil();
		opensil_xSIM_timepoint_1();
	}

	data_fabric_print_mmio_conf();

	fch_init(chip_info);
}

static void soc_final(void *chip_info)
{
	fch_final(chip_info);
}

struct chip_operations soc_amd_phoenix_ops = {
	.name = "AMD Phoenix SoC",
	.init = soc_init,
	.final = soc_final
};
