/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <device/device.h>
#include <drivers/amd/i2s_machine_dev/chip.h>
#include <drivers/i2c/generic/chip.h>
#include <drivers/usb/acpi/chip.h>
#include <ec/google/chromeec/ec.h>
#include <soc/gpio.h>
#include <soc/pci_devs.h>

extern struct chip_operations drivers_amd_i2s_machine_dev_ops;
extern struct chip_operations drivers_i2c_generic_ops;


static void update_hp_int_odl(void)
{

	static const struct device_path rt5682_path[] = {
		{
			.type = DEVICE_PATH_PCI,
			.pci.devfn = LPC_DEVFN
		},
		{
			.type = DEVICE_PATH_PNP,
			.pnp.port = 0xc09,
			.pnp.device = 0x0
		},
		{
			.type = DEVICE_PATH_GENERIC,
			.generic.id = 0,
			.generic.subid = 0
		},
		{
			.type = DEVICE_PATH_I2C,
			.i2c.device = 0x1a
		}
	};

	const struct device *rt5682_dev;
	struct drivers_i2c_generic_config *cfg;
	struct acpi_gpio *gpio;

	if (!variant_uses_codec_gpi())
		return;

	rt5682_dev = find_dev_nested_path(
		pci_root_bus(), rt5682_path, ARRAY_SIZE(rt5682_path));
	if (!rt5682_dev) {
		printk(BIOS_ERR, "%s: Failed to find audio device\n",
				__func__);
		return;
	}

	if (rt5682_dev->chip_ops != &drivers_i2c_generic_ops) {
		printk(BIOS_ERR, "%s: Incorrect device found\n", __func__);
		return;
	}

	cfg = config_of(rt5682_dev);
	gpio = &cfg->irq_gpio;
	gpio->pins[0] = 62;

}

void variant_audio_update(void)
{
	const struct device *gpp_a_dev;
	const struct device *acp_dev;
	struct device *machine_dev = NULL;

	if (variant_uses_v3_schematics())
		return;

	gpp_a_dev = pcidev_path_on_root(PCIE_GPP_A_DEVFN);
	if (gpp_a_dev == NULL)
		return;

	acp_dev = pcidev_path_behind(gpp_a_dev->link_list, AUDIO_DEVFN);
	if (acp_dev == NULL)
		return;

	while ((machine_dev = dev_bus_each_child(acp_dev->link_list, machine_dev)) != NULL) {
		struct drivers_amd_i2s_machine_dev_config *cfg;
		struct acpi_gpio *gpio;

		if (machine_dev->chip_info == NULL)
			continue;

		if (machine_dev->chip_ops != &drivers_amd_i2s_machine_dev_ops)
			continue;

		cfg = machine_dev->chip_info;
		gpio = &cfg->dmic_select_gpio;

		if (CONFIG(BOARD_GOOGLE_BASEBOARD_TREMBYLE))
			gpio->pins[0] = GPIO_13;
		else
			gpio->pins[0] = GPIO_6;

		break;
	}

	update_hp_int_odl();
}

static const struct device_path xhci0_bt_path[] = {
	{
		.type = DEVICE_PATH_PCI,
		.pci.devfn = PCIE_GPP_A_DEVFN
	},
	{
		.type = DEVICE_PATH_PCI,
		.pci.devfn = XHCI0_DEVFN
	},
	{
		.type = DEVICE_PATH_USB,
		.usb.port_type = 0,
		.usb.port_id = 0
	},
	{
		.type = DEVICE_PATH_USB,
		.usb.port_type = 2,
		.usb.port_id = 5
	}
};

static const struct device_path xhci1_bt_path[] = {
	{
		.type = DEVICE_PATH_PCI,
		.pci.devfn = PCIE_GPP_A_DEVFN
	},
	{
		.type = DEVICE_PATH_PCI,
		.pci.devfn = XHCI1_DEVFN
	},
	{
		.type = DEVICE_PATH_USB,
		.usb.port_type = 0,
		.usb.port_id = 0
	},
	{
		.type = DEVICE_PATH_USB,
		.usb.port_type = 2,
		.usb.port_id = 1
	}
};

/*
 * Removes reset_gpio from bluetooth device in device tree.
 *
 * The bluetooth device may be on XHCI0 or XHCI1 depending on SOC.
 * There's no harm in removing from both here.
 */
static void baseboard_remove_bluetooth_reset_gpio(void)
{
	const struct device *xhci0_bt_dev, *xhci1_bt_dev;
	struct drivers_usb_acpi_config *xhci0_bt_cfg, *xhci1_bt_cfg;

	xhci0_bt_dev = find_dev_nested_path(
		pci_root_bus(), xhci0_bt_path, ARRAY_SIZE(xhci0_bt_path));
	if (!xhci0_bt_dev) {
		printk(BIOS_ERR, "%s: Failed to find bluetooth device on XHCI0!", __func__);
		return;
	}
	/* config_of dies on failure, so a NULL check is not required */
	xhci0_bt_cfg = config_of(xhci0_bt_dev);
	xhci0_bt_cfg->reset_gpio.pin_count = 0;

	/* There's no bluetooth device on XHCI1 on Dalboz */
	if (CONFIG(BOARD_GOOGLE_BASEBOARD_DALBOZ))
		return;

	xhci1_bt_dev = find_dev_nested_path(
		pci_root_bus(), xhci1_bt_path, ARRAY_SIZE(xhci1_bt_path));
	if (!xhci1_bt_dev) {
		printk(BIOS_ERR, "%s: Failed to find bluetooth device on XHCI1!", __func__);
		return;
	}
	xhci1_bt_cfg = config_of(xhci1_bt_dev);
	xhci1_bt_cfg->reset_gpio.pin_count = 0;
}

void variant_bluetooth_update(void)
{
	if (CONFIG(BOARD_GOOGLE_BASEBOARD_DALBOZ) || variant_uses_v3_schematics())
		return;

	baseboard_remove_bluetooth_reset_gpio();
}
