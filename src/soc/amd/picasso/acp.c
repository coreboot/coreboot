/* SPDX-License-Identifier: GPL-2.0-only */

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
	write32((void *)(bar + ACP_I2S_PIN_CONFIG), cfg->acp_pin_cfg);

	if (cfg->acp_pin_cfg == I2S_PINS_I2S_TDM)
		sb_clk_output_48Mhz(); /* Internal connection to I2S */
}

static struct device_operations acp_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = init,
	.ops_pci = &pci_dev_ops_pci,
};

static const struct pci_driver acp_driver __pci_driver = {
	.ops = &acp_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_FAM17H_ACP,
};
