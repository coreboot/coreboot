/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpi.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <bootstate.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <ec/google/chromeec/ec.h>
#include <ec/ec.h>
#include <intelblocks/xhci.h>
#include <nhlt.h>
#include <smbios.h>
#include <soc/cpu.h>
#include <soc/gpio.h>
#include <soc/nhlt.h>
#include <soc/pci_devs.h>
#include <stdint.h>
#include <string.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <variant/ec.h>
#include <variant/gpio.h>

static bool is_cnvi_held_in_reset(void)
{
	struct device *dev = pcidev_path_on_root(PCH_DEVFN_CNVI);
	uint32_t reg = pci_read_config32(dev, PCI_VENDOR_ID);

	/*
	 * If vendor/device ID for CNVi reads as 0xffffffff, then it is safe to
	 * assume that it is being held in reset.
	 */
	if (reg == 0xffffffff)
		return true;

	return false;
}

static void disable_wifi_wake(void)
{
	static const struct pad_config wifi_wake_gpio[] = {
		PAD_NC(GPIO_119, UP_20K),
	};

	gpio_configure_pads(wifi_wake_gpio, ARRAY_SIZE(wifi_wake_gpio));
}

static void mainboard_init(void *chip_info)
{
	int boardid;
	const struct pad_config *base_pads;
	const struct pad_config *override_pads;
	size_t base_num, override_num;

	boardid = board_id();
	printk(BIOS_INFO, "Board ID: %d\n", boardid);

	base_pads = variant_base_gpio_table(&base_num);
	override_pads = variant_override_gpio_table(&override_num);

	gpio_configure_pads_with_override(base_pads, base_num,
			override_pads, override_num);

	if (!is_cnvi_held_in_reset())
		disable_wifi_wake();

	mainboard_ec_init();
}

static unsigned long mainboard_write_acpi_tables(
	struct device *device, unsigned long current, acpi_rsdp_t *rsdp)
{
	uintptr_t start_addr;
	uintptr_t end_addr;
	struct nhlt *nhlt;

	start_addr = current;

	nhlt = nhlt_init();

	if (nhlt == NULL)
		return start_addr;

	variant_nhlt_init(nhlt);

	end_addr = nhlt_soc_serialize(nhlt, start_addr);

	if (end_addr != start_addr)
		acpi_add_table(rsdp, (void *)start_addr);

	return end_addr;
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->write_acpi_tables = mainboard_write_acpi_tables;
	dev->ops->acpi_inject_dsdt_generator = chromeos_dsdt_generator;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};

void __weak variant_update_devtree(struct device *dev)
{
	/* Place holder for common updates. */
}

/*
 * Check if CNVi PCI device is released from reset. If yes, then the system is
 * booting with CNVi module. In this case, the PCIe device for WiFi needs to
 * be disabled. If CNVi device is held in reset, then disable it.
 */
static void wifi_device_update(void)
{
	struct device *dev;
	unsigned int devfn;

	if (is_cnvi_held_in_reset())
		devfn = PCH_DEVFN_CNVI;
	else
		devfn = PCH_DEVFN_PCIE1;

	dev = pcidev_path_on_root(devfn);
	if (dev)
		dev->enabled = 0;
}

void mainboard_devtree_update(struct device *dev)
{
	/* Apply common devtree updates. */
	wifi_device_update();

	/* Defer to variant for board-specific updates. */
	variant_update_devtree(dev);
}

const char *smbios_mainboard_manufacturer(void)
{
	static char oem_name[32];
	static const char *manuf;

	if (manuf)
		return manuf;

	if (google_chromeec_cbi_get_oem_name(&oem_name[0],
			ARRAY_SIZE(oem_name)) < 0) {
		printk(BIOS_ERR, "Couldn't obtain OEM name from CBI\n");
		manuf = CONFIG_MAINBOARD_SMBIOS_MANUFACTURER;
	} else {
		manuf = &oem_name[0];
	}

	return manuf;
}

bool __weak variant_ext_usb_status(unsigned int port_type, unsigned int port_id)
{
	/* All externally visible USB ports are present */
	return true;
}

static void disable_unused_devices(void *unused)
{
	usb_xhci_disable_unused(variant_ext_usb_status);
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_EXIT, disable_unused_devices, NULL);
