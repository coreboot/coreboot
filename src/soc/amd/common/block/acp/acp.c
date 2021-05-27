/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <amdblocks/acp.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/chip.h>
#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <commonlib/helpers.h>

/* ACP registers and associated fields */
#define ACP_I2S_PIN_CONFIG	0x1400	/* HDA, Soundwire, I2S */
#define  PIN_CONFIG_MASK	(7 << 0)
#define ACP_I2S_WAKE_EN		0x1414
#define  WAKE_EN_MASK		(1 << 0)
#define ACP_PME_EN		0x1418
#define  PME_EN_MASK		(1 << 0)

static void acp_update32(uintptr_t bar, uint32_t reg, uint32_t clear, uint32_t set)
{
	clrsetbits32((void *)(bar + reg), clear, set);
}

static void init(struct device *dev)
{
	const struct soc_amd_common_config *cfg = soc_get_common_config();
	struct resource *res;
	uintptr_t bar;

	res = dev->resource_list;
	if (!res || !res->base) {
		printk(BIOS_ERR, "Error, unable to configure pin in %s\n", __func__);
		return;
	}

	/* Set the proper I2S_PIN_CONFIG state */
	bar = (uintptr_t)res->base;
	acp_update32(bar, ACP_I2S_PIN_CONFIG, PIN_CONFIG_MASK, cfg->acp_config.acp_pin_cfg);

	/* Enable ACP_PME_EN and ACP_I2S_WAKE_EN for I2S_WAKE event */
	acp_update32(bar, ACP_I2S_WAKE_EN, WAKE_EN_MASK, !!cfg->acp_config.acp_i2s_wake_enable);
	acp_update32(bar, ACP_PME_EN, PME_EN_MASK, !!cfg->acp_config.acp_pme_enable);
}

static const char *acp_acpi_name(const struct device *dev)
{
	return "ACPD";
}

static void acp_fill_wov_method(const struct device *dev)
{
	const struct soc_amd_common_config *cfg = soc_get_common_config();
	const char *scope = acpi_device_path(dev);

	if (!cfg->acp_config.dmic_present || !scope)
		return;

	/* For ACP DMIC hardware runtime detection on the platform, _WOV method is populated. */
	acpigen_write_scope(scope); /* Scope */
	acpigen_write_method("_WOV", 0);
	acpigen_write_return_integer(1);
	acpigen_write_method_end();
	acpigen_write_scope_end();
}

static void acp_fill_ssdt(const struct device *dev)
{
	acpi_device_write_pci_dev(dev);
	acp_fill_wov_method(dev);
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
