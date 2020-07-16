/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "chip.h"
#include <soc/acp.h>
#include <soc/acpi.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include <amdblocks/acpimmio.h>
#include <commonlib/helpers.h>

static void acp_update32(uintptr_t bar, uint32_t reg, uint32_t and_mask, uint32_t or_mask)
{
	uint32_t val;

	val = read32((void *)(bar + reg));
	val &= ~and_mask;
	val |= or_mask;
	write32((void *)(bar + reg), val);
}

static void init(struct device *dev)
{
	const struct soc_amd_picasso_config *cfg;
	struct resource *res;
	uintptr_t bar;

	/* Set the proper I2S_PIN_CONFIG state */
	cfg = config_of_soc();

	res = dev->resource_list;
	if (!res || !res->base) {
		printk(BIOS_ERR, "Error, unable to configure pin in %s\n", __func__);
		return;
	}

	bar = (uintptr_t)res->base;
	acp_update32(bar, ACP_I2S_PIN_CONFIG, PIN_CONFIG_MASK, cfg->acp_pin_cfg);

	/* Enable ACP_PME_EN and ACP_I2S_WAKE_EN for I2S_WAKE event */
	acp_update32(bar, ACP_I2S_WAKE_EN, WAKE_EN_MASK, !!cfg->acp_i2s_wake_enable);
	acp_update32(bar, ACP_PME_EN, PME_EN_MASK, !!cfg->acpi_pme_enable);

	if (cfg->acp_pin_cfg == I2S_PINS_I2S_TDM)
		sb_clk_output_48Mhz(); /* Internal connection to I2S */
}

static const char *acp_acpi_name(const struct device *dev)
{
	return "ACPD";
}

#define AMD_I2S_ACPI_NAME	"I2SM"
#define AMD_I2S_ACPI_HID	"AMDI5682"
#define AMD_I2S_ACPI_DESC	"I2S machine driver"

static void acp_fill_i2s_machine_dev(const struct device *dev)
{
	const char *scope = acpi_device_path(dev);
	const struct soc_amd_picasso_config *cfg = config_of_soc();
	const struct acpi_gpio *dmic_select_gpio = &cfg->dmic_select_gpio;
	struct acpi_dp *dsd;

	if (dmic_select_gpio->pin_count == 0)
		return;

	acpigen_write_scope(scope); /* Scope */
	acpigen_write_device(AMD_I2S_ACPI_NAME); /* Device */
	acpigen_write_name_string("_HID", AMD_I2S_ACPI_HID);
	acpigen_write_name_integer("_UID", 1);
	acpigen_write_name_string("_DDN", AMD_I2S_ACPI_DESC);

	acpigen_write_STA(ACPI_STATUS_DEVICE_ALL_ON);

	/* Resources */
	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_gpio(dmic_select_gpio);
	acpigen_write_resourcetemplate_footer();

	dsd = acpi_dp_new_table("_DSD");
	/*
	 * This GPIO is used to select DMIC0 or DMIC1 by the kernel driver. It does not
	 * really have a polarity since low and high control the selection of DMIC and
	 * hence does not have an active polarity.
	 * Kernel driver does not use the polarity field and instead treats the GPIO
	 * selection as follows:
	 * Set low (0) = Select DMIC0
	 * Set high (1) = Select DMIC1
	 */
	acpi_dp_add_gpio(dsd, "dmic-gpios", acpi_device_path_join(dev, AMD_I2S_ACPI_NAME),
			 0,  /* Index = 0 (There is a single GPIO entry in _CRS). */
			 0,  /* Pin = 0 (There is a single pin in the GPIO resource). */
			 0); /* Active low = 0 (Kernel driver does not use active polarity). */
	acpi_dp_write(dsd);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */
}

static void acp_fill_ssdt(const struct device *dev)
{
	acpi_device_write_pci_dev(dev);
	acp_fill_i2s_machine_dev(dev);
}

static struct device_operations acp_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = init,
	.ops_pci = &pci_dev_ops_pci,
	.scan_bus = scan_static_bus,
	.acpi_name = acp_acpi_name,
	.acpi_fill_ssdt = acp_fill_ssdt,
};

static const struct pci_driver acp_driver __pci_driver = {
	.ops = &acp_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_FAM17H_ACP,
};
