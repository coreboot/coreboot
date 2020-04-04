/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "chip.h"
#include <soc/acpi.h>
#include <soc/pci_devs.h>
#include <soc/northbridge.h>
#include <soc/southbridge.h>
#include <amdblocks/acpimmio.h>
#include <commonlib/helpers.h>

static void enable(struct device *dev)
{
	const struct soc_amd_picasso_config *cfg;
	const struct device *nb_dev = pcidev_path_on_root(GNB_DEVFN);
	struct resource *res;
	uintptr_t bar;

	pci_dev_enable_resources(dev);

	/* Set the proper I2S_PIN_CONFIG state */
	if (!nb_dev || !nb_dev->chip_info)
		return;

	cfg = nb_dev->chip_info;

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

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations acp_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = enable,
	.ops_pci = &lops_pci,
};

static const struct pci_driver acp_driver __pci_driver = {
	.ops = &acp_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICD_ID_AMD_PCO_ACP,
};
