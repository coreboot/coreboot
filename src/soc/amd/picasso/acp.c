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
	acp_update32(bar, ACP_PME_EN, PME_EN_MASK, !!cfg->acp_pme_enable);

	if (cfg->acp_pin_cfg == I2S_PINS_I2S_TDM)
		sb_clk_output_48Mhz(); /* Internal connection to I2S */
}

static const char *acp_acpi_name(const struct device *dev)
{
	return "ACPD";
}

static struct device_operations acp_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = init,
	.ops_pci = &pci_dev_ops_pci,
	.scan_bus = scan_static_bus,
	.acpi_name = acp_acpi_name,
	.acpi_fill_ssdt = acpi_device_write_pci_dev,
};

static const struct pci_driver acp_driver __pci_driver = {
	.ops = &acp_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_FAM17H_ACP,
};
