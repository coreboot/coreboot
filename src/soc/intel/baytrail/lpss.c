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

#include <stdint.h>
#include <arch/io.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <reg_script.h>

#include <baytrail/iosf.h>
#include <baytrail/nvs.h>
#include <baytrail/pci_devs.h>
#include <baytrail/ramstage.h>

#include "chip.h"

static void dev_enable_acpi_mode(device_t dev, int iosf_reg, int nvs_index)
{
	struct reg_script ops[] = {
		/* Disable PCI interrupt, enable Memory and Bus Master */
		REG_PCI_OR32(PCI_COMMAND,
			     PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER | (1<<10)),
		/* Enable ACPI mode */
		REG_IOSF_OR(IOSF_PORT_LPSS, iosf_reg,
			    LPSS_CTL_PCI_CFG_DIS | LPSS_CTL_ACPI_INT_EN),
		REG_SCRIPT_END
	};
	struct resource *bar;
	global_nvs_t *gnvs;

	/* Find ACPI NVS to update BARs */
	gnvs = (global_nvs_t *)cbmem_find(CBMEM_ID_ACPI_GNVS);
	if (!gnvs) {
		printk(BIOS_ERR, "Unable to locate Global NVS\n");
		return;
	}

	/* Save BAR0 and BAR1 to ACPI NVS */
	bar = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (bar)
		gnvs->dev.lpss_bar0[nvs_index] = (u32)bar->base;

	bar = find_resource(dev, PCI_BASE_ADDRESS_1);
	if (bar)
		gnvs->dev.lpss_bar1[nvs_index] = (u32)bar->base;

	/* Device is enabled in ACPI mode */
	gnvs->dev.lpss_en[nvs_index] = 1;

	/* Put device in ACPI mode */
	reg_script_run_on_dev(dev, ops);
}

static void dev_enable_snoop_and_pm(device_t dev, int iosf_reg)
{
	struct reg_script ops[] = {
		REG_IOSF_RMW(IOSF_PORT_LPSS, iosf_reg,
		             ~(LPSS_CTL_SNOOP | LPSS_CTL_NOSNOOP),
		             LPSS_CTL_SNOOP | LPSS_CTL_PM_CAP_PRSNT),
		REG_SCRIPT_END,
	};

	reg_script_run_on_dev(dev, ops);
}

static void dev_ctl_reg(device_t dev, int *iosf_reg, int *nvs_index)
{
	*iosf_reg = -1;
	*nvs_index = -1;
#define SET_IOSF_REG(name_) \
	case PCI_DEVFN(name_ ## _DEV, name_ ## _FUNC): \
		*iosf_reg = LPSS_ ## name_ ## _CTL; \
		*nvs_index = LPSS_NVS_ ## name_

	switch (dev->path.pci.devfn) {
	SET_IOSF_REG(SIO_DMA1);
		break;
	SET_IOSF_REG(I2C1);
		break;
	SET_IOSF_REG(I2C2);
		break;
	SET_IOSF_REG(I2C3);
		break;
	SET_IOSF_REG(I2C4);
		break;
	SET_IOSF_REG(I2C5);
		break;
	SET_IOSF_REG(I2C6);
		break;
	SET_IOSF_REG(I2C7);
		break;
	SET_IOSF_REG(SIO_DMA2);
		break;
	SET_IOSF_REG(PWM1);
		break;
	SET_IOSF_REG(PWM2);
		break;
	SET_IOSF_REG(HSUART1);
		break;
	SET_IOSF_REG(HSUART2);
		break;
	SET_IOSF_REG(SPI);
		break;
	}
}

static void i2c_disable_resets(device_t dev)
{
	/* Release the I2C devices from reset. */
	static const struct reg_script ops[] = {
		REG_RES_WRITE32(PCI_BASE_ADDRESS_0, 0x804, 0x3),
		REG_SCRIPT_END,
	};

#define CASE_I2C(name_) \
	case PCI_DEVFN(name_ ## _DEV, name_ ## _FUNC)

	switch (dev->path.pci.devfn) {
	CASE_I2C(I2C1):
	CASE_I2C(I2C2):
	CASE_I2C(I2C3):
	CASE_I2C(I2C4):
	CASE_I2C(I2C5):
	CASE_I2C(I2C6):
	CASE_I2C(I2C7):
		printk(BIOS_DEBUG, "Releasing I2C device from reset.\n");
		reg_script_run_on_dev(dev, ops);
		break;
	default:
		return;
	}
}

static void lpss_init(device_t dev)
{
	struct soc_intel_baytrail_config *config = dev->chip_info;
	int iosf_reg, nvs_index;

	dev_ctl_reg(dev, &iosf_reg, &nvs_index);

	if (iosf_reg < 0) {
		int slot = PCI_SLOT(dev->path.pci.devfn);
		int func = PCI_FUNC(dev->path.pci.devfn);
		printk(BIOS_DEBUG, "Could not find iosf_reg for %02x.%01x\n",
		       slot, func);
		return;
	}
	dev_enable_snoop_and_pm(dev, iosf_reg);
	i2c_disable_resets(dev);

	if (config->lpss_acpi_mode)
		dev_enable_acpi_mode(dev, iosf_reg, nvs_index);
}

static struct device_operations device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= lpss_init,
	.enable			= NULL,
	.scan_bus		= NULL,
	.ops_pci		= &soc_pci_ops,
};

static const unsigned short pci_device_ids[] = {
	SIO_DMA1_DEVID,
	I2C1_DEVID,
	I2C2_DEVID,
	I2C3_DEVID,
	I2C4_DEVID,
	I2C5_DEVID,
	I2C6_DEVID,
	I2C7_DEVID,
	SIO_DMA2_DEVID,
	PWM1_DEVID,
	PWM2_DEVID,
	HSUART1_DEVID,
	HSUART2_DEVID,
	SPI_DEVID,
	0,
};

static const struct pci_driver southcluster __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VENDOR_ID_INTEL,
	.devices	= pci_device_ids,
};
