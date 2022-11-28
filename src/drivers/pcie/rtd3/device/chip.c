/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <assert.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "chip.h"

/*
 * Writes the ACPI power resources for a PCI device so it can enter D3Cold.
 *
 * If the device is a storage class, then the StorageD3Enable _DSD will
 * also be added.
 *
 * e.g.,
 *
 *    Scope (\_SB.PCI0.GP14)
 *    {
 *        Device (NVME)
 *        {
 *            Name (_PR0, Package (0x01)  // _PR0: Power Resources for D0
 *            {
 *                PRIC
 *            })
 *            Name (_PR3, Package (0x01)  // _PR3: Power Resources for D3hot
 *            {
 *                PRIC
 *            })
 *            PowerResource (PRIC, 0x00, 0x0000)
 *            {
 *                Method (_STA, 0, NotSerialized)  // _STA: Status
 *                {
 *                    ...
 *                }
 *
 *                Method (_ON, 0, Serialized)  // _ON_: Power On
 *                {
 *                    ...
 *                }
 *
 *                Method (_OFF, 0, Serialized)  // _OFF: Power Off
 *                {
 *                    ...
 *                }
 *            }
 *
 *            Name (_ADR, 0x0000000000000000)  // _ADR: Address
 *            Method (_STA, 0, NotSerialized)  // _STA: Status
 *            {
 *                Return (0x0F)
 *            }
 *
 *            Name (_DSD, Package (0x02)  // _DSD: Device-Specific Data
 *            {
 *                ToUUID ("5025030f-842f-4ab4-a561-99a5189762d0"),
 *                Package (0x01)
 *                {
 *                    Package (0x02)
 *                    {
 *                        "StorageD3Enable",
 *                        One
 *                    }
 *                }
 *            })
 *        }
 *    }
 */
static void pcie_rtd3_device_acpi_fill_ssdt(const struct device *dev)
{
	const struct drivers_pcie_rtd3_device_config *config = config_of(dev);
	/* Copy the GPIOs to avoid discards 'const' qualifier error */
	struct acpi_gpio reset_gpio = config->reset_gpio;
	struct acpi_gpio enable_gpio = config->enable_gpio;
	const struct acpi_power_res_params power_res_params = {
		.reset_gpio		= &reset_gpio,
		.reset_delay_ms		= config->reset_delay_ms,
		.reset_off_delay_ms	= config->reset_off_delay_ms,
		.enable_gpio		= &enable_gpio,
		.enable_delay_ms	= config->enable_delay_ms,
		.enable_off_delay_ms	= config->enable_off_delay_ms,
		.use_gpio_for_status	= true,
	};
	const char *scope = acpi_device_scope(dev);
	const char *name = acpi_device_name(dev);

	assert(name);
	assert(scope);

	printk(BIOS_INFO, "%s.%s: Enable RTD3 for %s (%s)\n", scope, name, dev_path(dev),
	       dev->chip_ops->name);

	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));

	acpi_device_add_power_res(&power_res_params);

	acpigen_write_ADR_pci_device(dev);
	acpigen_write_STA(acpi_device_status(dev));

	/* Storage devices won't enter D3 without this property */
	if ((dev->class >> 16) == PCI_BASE_CLASS_STORAGE) {
		acpi_device_add_storage_d3_enable(NULL);

		printk(BIOS_INFO, "%s.%s: Added StorageD3Enable property\n", scope, name);
	}

	acpigen_write_device_end();
	acpigen_write_scope_end();

	/* Call the default PCI acpi_fill_ssdt */
	pci_rom_ssdt(dev);
}

static const char *pcie_rtd3_device_acpi_name(const struct device *dev)
{
	const struct drivers_pcie_rtd3_device_config *config = config_of(dev);

	return config->name;
}

static struct device_operations pcie_rtd3_device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= pci_dev_init,
	.ops_pci		= &pci_dev_ops_pci,
	.write_acpi_tables	= pci_rom_write_acpi_tables,
	.acpi_fill_ssdt		= pcie_rtd3_device_acpi_fill_ssdt,
	.acpi_name		= pcie_rtd3_device_acpi_name,
};

static void pcie_rtd3_device_enable(struct device *dev)
{
	struct drivers_pcie_rtd3_device_config *config = dev ? dev->chip_info : NULL;

	if (!config)
		return;

	if (dev->path.type != DEVICE_PATH_PCI) {
		printk(BIOS_ERR, "%s: Invalid device type\n", dev_path(dev));
		return;
	}

	dev->ops = &pcie_rtd3_device_ops;
}

struct chip_operations drivers_pcie_rtd3_device_ops = {
	CHIP_NAME("PCIe Device w/ Runtime D3")
	.enable_dev = pcie_rtd3_device_enable
};
