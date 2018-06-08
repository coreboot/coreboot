/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#include <stdint.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <reg_script.h>

#include <soc/iosf.h>
#include <soc/nvs.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include "chip.h"

static const struct reg_script emmc_ops[] = {
	/* Enable 2ms card stable feature. */
	REG_PCI_OR32(0xa8, (1 << 24)),
	/* Enable HS200 */
	REG_PCI_WRITE32(0xa0, 0x446cc801),
	REG_PCI_WRITE32(0xa4, 0x80000807),
	/* cfio_regs_score_special_bits.sdio1_dummy_loopback_en=1 */
	REG_IOSF_OR(IOSF_PORT_SCORE, 0x49c0, (1 << 3)),
	/* CLKGATE_EN_1 . cr_scc_mipihsi_clkgate_en  = 1 */
	REG_IOSF_RMW(IOSF_PORT_CCU, 0x1c, ~(3 << 26), (1 << 26)),
	/* Set slew for HS200 */
	REG_IOSF_RMW(IOSF_PORT_SCORE, 0x48c0, ~0x3c, 0x3c),
	REG_IOSF_RMW(IOSF_PORT_SCORE, 0x48c4, ~0x3c, 0x3c),
	/* Max timeout */
	REG_RES_WRITE8(PCI_BASE_ADDRESS_0, 0x002e, 0x0e),
	REG_SCRIPT_END,
};

static void emmc_init(struct device *dev)
{
	struct soc_intel_baytrail_config *config = dev->chip_info;

	printk(BIOS_DEBUG, "eMMC init\n");
	reg_script_run_on_dev(dev, emmc_ops);

	if (config->scc_acpi_mode)
		scc_enable_acpi_mode(dev, SCC_MMC_CTL, SCC_NVS_MMC);
}

static struct device_operations device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= emmc_init,
	.enable			= NULL,
	.scan_bus		= NULL,
	.ops_pci		= &soc_pci_ops,
};

static const struct pci_driver southcluster __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VENDOR_ID_INTEL,
	.device		= MMC_DEVID,
};
