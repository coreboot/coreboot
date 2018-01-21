/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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

#include <arch/io.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <reg_script.h>

#include <soc/iomap.h>
#include <soc/iosf.h>
#include <soc/lpc.h>
#include <soc/nvs.h>
#include <soc/pattrs.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/ramstage.h>
#include "chip.h"


/*
 * The LPE audio devices needs 1MiB of memory reserved aligned to a 512MiB
 * address. Just take 1MiB @ 512MiB.
 */
#define FIRMWARE_PHYS_BASE (512 << 20)
#define FIRMWARE_PHYS_LENGTH (2 << 20)
#define FIRMWARE_PCI_REG_BASE 0xa8
#define FIRMWARE_PCI_REG_LENGTH 0xac
#define FIRMWARE_REG_BASE_C0 0x144000
#define FIRMWARE_REG_LENGTH_C0 (FIRMWARE_REG_BASE_C0 + 4)

static void assign_device_nvs(device_t dev, u32 *field, unsigned int index)
{
	struct resource *res;

	res = find_resource(dev, index);
	if (res)
		*field = res->base;
}

static void lpe_enable_acpi_mode(device_t dev)
{
	static const struct reg_script ops[] = {
		/* Disable PCI interrupt, enable Memory and Bus Master */
		REG_PCI_OR32(PCI_COMMAND,
			     PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER
			     | PCI_COMMAND_INT_DISABLE),
		/* Enable ACPI mode */
		REG_IOSF_OR(IOSF_PORT_0x58, LPE_PCICFGCTR1,
			    LPE_PCICFGCTR1_PCI_CFG_DIS |
			    LPE_PCICFGCTR1_ACPI_INT_EN),
		REG_SCRIPT_END
	};
	global_nvs_t *gnvs;

	/* Find ACPI NVS to update BARs */
	gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);
	if (!gnvs) {
		printk(BIOS_ERR, "Unable to locate Global NVS\n");
		return;
	}

	/* Save BAR0, BAR1, and firmware base  to ACPI NVS */
	assign_device_nvs(dev, &gnvs->dev.lpe_bar0, PCI_BASE_ADDRESS_0);
	/* LPE seems does not have BAR at PCI_BASE_ADDRESS_1 so disable it. */
	/* assign_device_nvs(dev, &gnvs->dev.lpe_bar1, PCI_BASE_ADDRESS_1);  */
	assign_device_nvs(dev, &gnvs->dev.lpe_fw, FIRMWARE_PCI_REG_BASE);

	/* Device is enabled in ACPI mode */
	gnvs->dev.lpe_en = 1;

	/* Put device in ACPI mode */
	reg_script_run_on_dev(dev, ops);
}

static void setup_codec_clock(device_t dev)
{
	uint32_t reg;
	u32 *clk_reg;
	struct soc_intel_braswell_config *config;
	const char *freq_str;

	config = dev->chip_info;
	switch (config->lpe_codec_clk_src) {
	case LPE_CLK_SRC_XTAL:
		/* XTAL driven bit2=0 */
		freq_str = "19.2MHz External Crystal";
		reg = CLK_SRC_XTAL;
		break;
	case LPE_CLK_SRC_PLL:
		/* PLL driven bit2=1 */
		freq_str = "19.2MHz PLL";
		reg = CLK_SRC_PLL;
		break;
	default:
		reg = CLK_SRC_XTAL;
		printk(BIOS_DEBUG, "LPE codec clock default to using Crystal\n");
		return;
	}

	/* Default to always running. */
	reg |= CLK_CTL_ON;


	printk(BIOS_DEBUG, "LPE Audio codec clock set to %sMHz.\n", freq_str);

	clk_reg = (u32 *) (PMC_BASE_ADDRESS + PLT_CLK_CTL_0);

	write32(clk_reg, (read32(clk_reg) & ~0x7) | reg);
}

static void lpe_stash_firmware_info(device_t dev)
{
	struct resource *res;
	struct resource *mmio;

	res = find_resource(dev, FIRMWARE_PCI_REG_BASE);
	if (res == NULL) {
		printk(BIOS_DEBUG, "LPE Firmware memory not found.\n");
		return;
	}
	printk(BIOS_DEBUG, "LPE FW Resource: 0x%08x\n", (u32) res->base);

	/* Continue using old way of informing firmware address / size. */
	pci_write_config32(dev, FIRMWARE_PCI_REG_BASE, res->base);
	pci_write_config32(dev, FIRMWARE_PCI_REG_LENGTH, res->size);

	/* Also put the address in MMIO space like on C0 BTM */
	mmio = find_resource(dev, PCI_BASE_ADDRESS_0);
	write32((void *)(uintptr_t)(mmio->base + FIRMWARE_REG_BASE_C0),
		res->base);
	write32((void *)(uintptr_t)(mmio->base + FIRMWARE_REG_LENGTH_C0),
		res->size);
}


static void lpe_init(device_t dev)
{
	struct soc_intel_braswell_config *config = dev->chip_info;

	printk(BIOS_SPEW, "%s/%s ( %s )\n",
			__FILE__, __func__, dev_name(dev));

	lpe_stash_firmware_info(dev);
	setup_codec_clock(dev);

	if (config->lpe_acpi_mode)
		lpe_enable_acpi_mode(dev);
}

static void lpe_read_resources(device_t dev)
{
	pci_dev_read_resources(dev);

	reserved_ram_resource(dev, FIRMWARE_PCI_REG_BASE,
			      FIRMWARE_PHYS_BASE >> 10,
			      FIRMWARE_PHYS_LENGTH >> 10);
}

static const struct device_operations device_ops = {
	.read_resources		= lpe_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
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
