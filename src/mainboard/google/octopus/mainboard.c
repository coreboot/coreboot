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
#include <compiler.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <ec/google/chromeec/ec.h>
#include <ec/ec.h>
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
	struct device *dev = dev_find_slot(0, PCH_DEVFN_CNVI);
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

#define SKU_UNKNOWN		0xFFFFFFFF
#define SKU_MAX			255

static uint32_t get_board_sku(void)
{
	static uint32_t sku_id = SKU_UNKNOWN;

	if (sku_id != SKU_UNKNOWN)
		return sku_id;

	if (google_chromeec_cbi_get_sku_id(&sku_id))
		sku_id = SKU_UNKNOWN;

	return sku_id;
}

const char *smbios_mainboard_sku(void)
{
	static char sku_str[7]; /* sku{0..255} */
	uint32_t sku_id = get_board_sku();

	if ((sku_id == SKU_UNKNOWN) || (sku_id > SKU_MAX)) {
		printk(BIOS_ERR, "%s: Unexpected SKU ID %u\n",
			__func__, sku_id);
		return "";
	}

	snprintf(sku_str, sizeof(sku_str), "sku%u", sku_id);

	return sku_str;
}

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

	dev = dev_find_slot(0, devfn);
	dev->enabled = 0;
}

void mainboard_devtree_update(struct device *dev)
{
	/* Apply common devtree updates. */
	wifi_device_update();

	/* Defer to variant for board-specific updates. */
	variant_update_devtree(dev);
}
