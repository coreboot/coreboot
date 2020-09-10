/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <device/device.h>
#include <drivers/amd/i2s_machine_dev/chip.h>
#include <drivers/i2c/generic/chip.h>
#include <drivers/i2c/hid/chip.h>
#include <drivers/usb/acpi/chip.h>
#include <ec/google/chromeec/ec.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>

extern struct chip_operations drivers_amd_i2s_machine_dev_ops;
extern struct chip_operations drivers_i2c_generic_ops;
extern struct chip_operations drivers_i2c_hid_ops;

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
	struct soc_amd_picasso_config *soc_cfg;

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

	/*
	 * When using CODEC_GPI for headphone jack interrupt, ACP_PME_EN and ACP_I2S_WAKE_EN
	 * need to be set to trigger I2S_WAKE event for headphone jack.
	 */
	soc_cfg = config_of_soc();
	soc_cfg->acp_i2s_wake_enable = 1;
	soc_cfg->acp_pme_enable = 1;
}

static void update_dmic_gpio(void)
{
	static const struct device_path acp_machine_path[] = {
		{
			.type = DEVICE_PATH_PCI,
			.pci.devfn = PCIE_GPP_A_DEVFN
		},
		{
			.type = DEVICE_PATH_PCI,
			.pci.devfn = AUDIO_DEVFN
		},
		{
			.type = DEVICE_PATH_GENERIC,
			.generic.id = 0,
			.generic.subid = 0
		}
	};

	const struct device *machine_dev;
	struct drivers_amd_i2s_machine_dev_config *cfg;
	struct acpi_gpio *gpio;

	if (variant_uses_v3_schematics())
		return;

	machine_dev = find_dev_nested_path(
		pci_root_bus(), acp_machine_path, ARRAY_SIZE(acp_machine_path));
	if (!machine_dev) {
		printk(BIOS_ERR, "%s: Failed to find ACP machine device\n", __func__);
		return;
	}

	if (machine_dev->chip_ops != &drivers_amd_i2s_machine_dev_ops) {
		printk(BIOS_ERR, "%s: Incorrect device found\n", __func__);
		return;
	}

	cfg = config_of(machine_dev);
	gpio = &cfg->dmic_select_gpio;

	if (CONFIG(BOARD_GOOGLE_BASEBOARD_TREMBYLE))
		gpio->pins[0] = GPIO_13;
	else
		gpio->pins[0] = GPIO_6;

}

void variant_audio_update(void)
{
	update_dmic_gpio();
	update_hp_int_odl();
}

/*
 * Removes reset_gpio from usb device in device tree.
 *
 * debug_device_name is used for debug messaging only.
 */
static void remove_usb_device_reset_gpio(const struct device_path usb_path[],
					 size_t path_length, const char *debug_device_name)
{

	const struct device *usb_dev;
	struct drivers_usb_acpi_config *usb_cfg;

	usb_dev = find_dev_nested_path(pci_root_bus(), usb_path, path_length);
	if (!usb_dev) {
		printk(BIOS_ERR, "%s: Failed to find %s!", __func__, debug_device_name);
		return;
	}
	/* config_of dies on failure, so a NULL check is not required */
	usb_cfg = config_of(usb_dev);
	usb_cfg->reset_gpio.pin_count = 0;
}

/*
 * The bluetooth device may be on XHCI0 or XHCI1 depending on SOC.
 * There's no harm in removing reset_gpio from both here.
 */
static void baseboard_trembyle_remove_bluetooth_reset_gpio(void)
{
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

	remove_usb_device_reset_gpio(xhci0_bt_path, ARRAY_SIZE(xhci0_bt_path),
				     "XHCI0 Bluetoth USB Device");
	remove_usb_device_reset_gpio(xhci1_bt_path, ARRAY_SIZE(xhci1_bt_path),
				     "XHCI1 Bluetoth USB Device");
}

void variant_bluetooth_update(void)
{
	if (CONFIG(BOARD_GOOGLE_BASEBOARD_DALBOZ) || variant_uses_v3_schematics())
		return;

	baseboard_trembyle_remove_bluetooth_reset_gpio();
}

void variant_touchscreen_update(void)
{
	DEVTREE_CONST struct device *mmio_dev = NULL;
	struct device *child = NULL;

	/*
	 * By default, devicetree/overridetree entries for touchscreen device are configured to
	 * match v3.6 of reference schematics. So, if the board is using v3.6+ schematics, no
	 * additional work is required here. For maintaining support for pre-v3.6 boards, rest
	 * of the code in this function finds all entries that correspond to touchscreen
	 * devices (identified by reset_gpio being set to GPIO_140) and updates them as per
	 * pre-v3.6 version of schematics:
	 * 1. reset_gpio is marked as active high.
	 */
	if (variant_uses_v3_6_schematics())
		return;

	while (1) {
		mmio_dev = dev_find_path(mmio_dev, DEVICE_PATH_MMIO);
		if (mmio_dev == NULL)
			break;
		if (mmio_dev->path.mmio.addr == APU_I2C2_BASE)
			break;
	}

	if (mmio_dev == NULL)
		return;

	while ((child = dev_bus_each_child(mmio_dev->link_list, child)) != NULL) {
		struct drivers_i2c_generic_config *cfg;

		if (child->chip_ops == &drivers_i2c_generic_ops) {
			cfg = config_of(child);
		} else if (child->chip_ops == &drivers_i2c_hid_ops) {
			struct drivers_i2c_hid_config *hid_cfg;
			hid_cfg = config_of(child);
			cfg = &hid_cfg->generic;
		} else {
			continue;
		}

		/* If reset_gpio is set to GPIO_140, assume that this is touchscreen device. */
		if (cfg->reset_gpio.pins[0] != GPIO_140)
			continue;

		cfg->reset_gpio.active_low = 0;
		cfg->enable_gpio.pin_count = 0;
		cfg->enable_gpio.pins[0] = 0;
	}
}
