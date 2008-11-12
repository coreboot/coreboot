/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <msr.h>
#include <legacy.h>
#include <device/pci_ids.h>
#include <statictree.h>
#include <config.h>
#include "i82801gx.h"

typedef struct southbridge_intel_i82801gx_ide_dts_config config_t;

static void ide_init(struct device *dev)
{
	u16 ideTimingConfig;
	u32 reg32;

	/* Get the chip configuration */
	config_t *config = dev->device_configuration;

	int enable_primary = config->ide_enable_primary;
	int enable_secondary = config->ide_enable_secondary;

	reg32 = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND, reg32 | PCI_COMMAND_IO | PCI_COMMAND_MASTER);

	/* Native Capable, but not enabled. */
	pci_write_config8(dev, 0x09, 0x8a);

	ideTimingConfig = pci_read_config16(dev, IDE_TIM_PRI);
	ideTimingConfig &= ~IDE_DECODE_ENABLE;
	ideTimingConfig |= IDE_SITRE;
	if (enable_primary) {
		/* Enable primary IDE interface. */
		ideTimingConfig |= IDE_DECODE_ENABLE;
		ideTimingConfig |= (2 << 12); // ISP = 3 clocks
		ideTimingConfig |= (3 << 8); // RCT = 1 clock
		ideTimingConfig |= (1 << 1); // IE0
		ideTimingConfig |= (1 << 0); // TIME0
		printk(BIOS_DEBUG, "IDE0 ");
	}
	pci_write_config16(dev, IDE_TIM_PRI, ideTimingConfig);

	ideTimingConfig = pci_read_config16(dev, IDE_TIM_SEC);
	ideTimingConfig &= ~IDE_DECODE_ENABLE;
	ideTimingConfig |= IDE_SITRE;
	if (enable_secondary) {
		/* Enable secondary IDE interface. */
		ideTimingConfig |= IDE_DECODE_ENABLE;
		ideTimingConfig |= (2 << 12); // ISP = 3 clocks
		ideTimingConfig |= (3 << 8); // RCT = 1 clock
		ideTimingConfig |= (1 << 1); // IE0
		ideTimingConfig |= (1 << 0); // TIME0
		printk(BIOS_DEBUG, "IDE1 ");
	}
	pci_write_config16(dev, IDE_TIM_SEC, ideTimingConfig);

	/* Set IDE I/O Configuration */
	if (enable_secondary)
		reg32 = SIG_MODE_NORMAL | FAST_PCB1 | FAST_PCB0 | PCB1 | PCB0;
	else
		reg32 = SIG_MODE_NORMAL | FAST_PCB1 | PCB1;
	pci_write_config32(dev, IDE_CONFIG, reg32);

	/* Set Interrupt Line */
	/* Interrupt Pin is set by D31IP.PIP */
	pci_write_config32(dev, INTR_LN, 0xff); /* Int 15 */
}
void i82801gx_enable(struct device * dev);
struct device_operations i82801gx_ide = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_INTEL,
			      .device = 0x27df}}},
	.constructor		 = default_device_constructor,
	.reset_bus		= pci_bus_reset,
	.phase3_chip_setup_dev	= i82801gx_enable,
	.phase3_scan		 = pci_scan_bridge,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_bus_enable_resources,
	.phase6_init		 = ide_init,
	.ops_pci		 = &pci_dev_ops_pci,
};
