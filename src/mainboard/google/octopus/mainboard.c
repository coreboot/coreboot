/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <baseboard/cbi_ssfc.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <bootstate.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <drivers/i2c/generic/chip.h>
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
#include <variant/ec.h>
#include <variant/gpio.h>

extern struct chip_operations drivers_i2c_generic_ops;
extern struct chip_operations drivers_i2c_cs42l42_ops;
extern struct chip_operations drivers_i2c_da7219_ops;

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

/*
 * GPIO_137 for two audio codecs right now has the different configuration so
 * if SSFC indicates that codec is different than default one then GPIO_137
 * needs to be overridden for the corresponding second source.
 */
static void gpio_modification_by_ssfc(struct pad_config *table, size_t num)
{
	/* For RT5682, GPIO 137 should be set as EDGE_BOTH. */
	const struct pad_config rt5682_gpio_137 = PAD_CFG_GPI_APIC_IOS(GPIO_137,
			NONE, DEEP, EDGE_BOTH, INVERT, HIZCRx1, DISPUPD);
	enum ssfc_audio_codec codec = ssfc_get_audio_codec();

	if (table == NULL || num == 0)
		return;

	/*
	 * Currently we only have the case of RT5682 as the second source. And
	 * in case of Ampton which used RT5682 as the default source, it didn't
	 * provide override_table right now so it will be returned earlier since
	 * table above is NULL.
	 */
	if ((codec != SSFC_AUDIO_CODEC_RT5682) &&
		(codec != SSFC_AUDIO_CODEC_RT5682_VS))
		return;

	while (num--) {
		if (table->pad == GPIO_137) {
			*table = rt5682_gpio_137;
			printk(BIOS_INFO,
				"Configure GPIO 137 based on SSFC.\n");
			return;
		}

		table++;
	}
}

static void mainboard_init(void *chip_info)
{
	int boardid;
	const struct pad_config *base_pads;
	const struct pad_config *override_pads;
	size_t base_num, override_num;

	boardid = board_id();
	printk(BIOS_INFO, "Board ID: %d\n", boardid);

	base_pads = baseboard_gpio_table(&base_num);
	override_pads = variant_override_gpio_table(&override_num);
	gpio_modification_by_ssfc((struct pad_config *)override_pads,
			override_num);

	gpio_configure_pads_with_override(base_pads, base_num,
			override_pads, override_num);

	if (!is_cnvi_held_in_reset())
		disable_wifi_wake();

	mainboard_ec_init();
}

static unsigned long mainboard_write_acpi_tables(
	const struct device *device, unsigned long current, acpi_rsdp_t *rsdp)
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

/*
 * Base on SSFC value in the CBI from EC to enable one of audio codec sources in
 * the device tree.
 */
static void audio_codec_device_update(void)
{
	struct device *audio_dev = NULL;
	struct bus *audio_i2c_bus =
		pcidev_path_on_root(PCH_DEVFN_I2C5)->link_list;
	enum ssfc_audio_codec codec = ssfc_get_audio_codec();

	while ((audio_dev = dev_bus_each_child(audio_i2c_bus, audio_dev))) {
		if (audio_dev->chip_info == NULL)
			continue;

		if ((audio_dev->chip_ops == &drivers_i2c_da7219_ops) &&
			(codec == SSFC_AUDIO_CODEC_DA7219)) {
			printk(BIOS_INFO, "enable DA7219.\n");
			continue;
		}

		if (audio_dev->chip_ops == &drivers_i2c_generic_ops) {
			struct drivers_i2c_generic_config *cfg =
				audio_dev->chip_info;

			if ((cfg != NULL && !strcmp(cfg->hid, "10EC5682")) &&
				(codec == SSFC_AUDIO_CODEC_RT5682)) {
				printk(BIOS_INFO, "enable RT5682 VD.\n");
				continue;
			}

			if ((cfg != NULL && !strcmp(cfg->hid, "10EC5682")) &&
				(codec == SSFC_AUDIO_CODEC_RT5682_VS)) {
				cfg->hid = "RTL5682";
				printk(BIOS_INFO, "enable RT5682 VS.\n");
				continue;
			}

			if ((cfg != NULL && !strcmp(cfg->hid, "RTL5682")) &&
				(codec == SSFC_AUDIO_CODEC_RT5682_VS)) {
				printk(BIOS_INFO, "enable RT5682 VS.\n");
				continue;
			}
		}

		if ((audio_dev->chip_ops == &drivers_i2c_cs42l42_ops) &&
			(codec == SSFC_AUDIO_CODEC_CS42L42)) {
			printk(BIOS_INFO, "enable CS42L42.\n");
			continue;
		}

		printk(BIOS_INFO, "%s has been disabled\n", audio_dev->chip_ops->name);
		audio_dev->enabled = 0;
	}
}

void mainboard_devtree_update(struct device *dev)
{
	/* Apply common devtree updates. */
	wifi_device_update();
	audio_codec_device_update();

	/* Defer to variant for board-specific updates. */
	variant_update_devtree(dev);
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
