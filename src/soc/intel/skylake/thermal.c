/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corp.
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

#include <device/mmio.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/thermal.h>

#include "chip.h"

#define MAX_TRIP_TEMP 205
#define DEFAULT_TRIP_TEMP 50

static void *pch_thermal_get_bar(struct device *dev)
{
	uintptr_t bar;

	bar = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
	/*
	 * Bits [31:12] are the base address as per EDS for Thermal Device,
	 * Don't care about [11:0] bits
	 */
	return (void *)(bar & ~PCI_BASE_ADDRESS_MEM_ATTR_MASK);
}

static void pch_thermal_set_bar(struct device *dev, uintptr_t tempbar)
{
	uint8_t pcireg;

	/* Assign Resources to Thermal Device */
	/* Clear BIT 1-2 of Command Register */
	pcireg = pci_read_config8(dev, PCI_COMMAND);
	pcireg &= ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
	pci_write_config8(dev, PCI_COMMAND, pcireg);

	/* Program Temporary BAR for Thermal Device */
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, tempbar);
	pci_write_config32(dev, PCI_BASE_ADDRESS_1, 0x0);

	/* Enable Bus Master and MMIO Space */
	pcireg = pci_read_config8(dev, PCI_COMMAND);
	pcireg |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;
	pci_write_config8(dev, PCI_COMMAND, pcireg);
}

/* PCH Low Temp Threshold (LTT) */
static uint16_t pch_get_ltt_value(struct device *dev)
{
	static struct soc_intel_skylake_config *config;
	uint16_t ltt_value;
	uint16_t trip_temp = DEFAULT_TRIP_TEMP;

	config = dev->chip_info;

	if (config->pch_trip_temp)
		trip_temp = config->pch_trip_temp;

	if (trip_temp > MAX_TRIP_TEMP)
		die("Input PCH temp trip is higher than allowed range!");

	/* Trip Point Temp = (LTT / 2 - 50 degree C) */
	ltt_value = (trip_temp + 50) * 2;

	return ltt_value;
}

/* Enable thermal sensor power management */
void pch_thermal_configuration(void)
{
	uint16_t reg16;
	struct device *dev = PCH_DEV_THERMAL;
	if (!dev) {
		printk(BIOS_ERR, "PCH_DEV_THERMAL device not found!\n");
		return;
	}
	void *thermalbar = pch_thermal_get_bar(dev);

	/* Use default pre-ram bar */
	if (!thermalbar) {
		pch_thermal_set_bar(dev, THERMAL_BASE_ADDRESS);
		thermalbar = (void *)THERMAL_BASE_ADDRESS;
	}

	/* Set Low Temp Threshold (LTT) at TSPM offset 0x1c[8:0] */
	reg16 = read16(thermalbar + THERMAL_SENSOR_POWER_MANAGEMENT);
	reg16 &= ~0x1ff;
	/* Low Temp Threshold (LTT) */
	reg16 |= pch_get_ltt_value(dev);
	write16(thermalbar + THERMAL_SENSOR_POWER_MANAGEMENT, reg16);
}
