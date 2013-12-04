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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include <baytrail/iomap.h>
#include <baytrail/pci_devs.h>
#include <baytrail/pmc.h>
#include <baytrail/ramstage.h>
#include "chip.h"


static void setup_codec_clock(device_t dev)
{
	uint32_t reg;
	int clk_reg;
	struct soc_intel_baytrail_config *config;
	const char *freq_str;

	config = dev->chip_info;
	switch (config->lpe_codec_clk_freq) {
	case 19:
		freq_str = "19.2";
		reg = CLK_FREQ_19P2MHZ;
		break;
	case 25:
		freq_str = "25";
		reg = CLK_FREQ_25MHZ;
		break;
	default:
		printk(BIOS_DEBUG, "LPE codec clock not required.\n");
		return;
	}

	/* Default to always running. */
	reg |= CLK_CTL_ON;

	if (config->lpe_codec_clk_num < 0 || config->lpe_codec_clk_num > 5) {
		printk(BIOS_DEBUG, "Invalid LPE codec clock number.\n");
		return;
	}

	printk(BIOS_DEBUG, "LPE Audio codec clock set to %sMHz.\n", freq_str);

	clk_reg = PMC_BASE_ADDRESS + PLT_CLK_CTL_0;
	clk_reg += 4 * config->lpe_codec_clk_num;

	write32(clk_reg, (read32(clk_reg) & ~0x7) | reg);
}

static void lpe_init(device_t dev)
{
	setup_codec_clock(dev);
}

static const struct device_operations device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= NULL,
	.init			= lpe_init,
	.enable			= NULL,
	.scan_bus		= NULL,
	.ops_pci		= &soc_pci_ops,
};

static const struct pci_driver southcluster __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VENDOR_ID_INTEL,
	.device		= LPE_DEVID,
};
