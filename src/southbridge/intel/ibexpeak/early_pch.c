/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 * Copyright (C) 2013 Vladimir Serbinenko <phcoder@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <device/pci_ops.h>
#include <northbridge/intel/nehalem/nehalem.h>
#include <southbridge/intel/ibexpeak/pch.h>
#include <southbridge/intel/common/gpio.h>

#include "chip.h"

static void early_lpc_init(void)
{
	const struct device *dev = pcidev_on_root(0x1f, 0);
	const struct southbridge_intel_ibexpeak_config *config = NULL;

	/* Add some default decode ranges:
	   - 0x2e/2f, 0x4e/0x4f
	   - EC/Mouse/KBC 60/64, 62/66
	   - 0x3f8 COMA
	   If more are needed, update in mainboard_lpc_init hook
	*/
	pci_write_config16(PCH_LPC_DEV, LPC_EN,
			   CNF2_LPC_EN | CNF1_LPC_EN | MC_LPC_EN | KBC_LPC_EN |
			   COMA_LPC_EN);
	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x10);

	/* Clear PWR_FLR */
	pci_write_config8(PCH_LPC_DEV, GEN_PMCON_3,
			  (pci_read_config8(PCH_LPC_DEV, GEN_PMCON_3) & ~2) | 1);

	pci_write_config32(PCH_LPC_DEV, ETR3,
			   pci_read_config32(PCH_LPC_DEV, ETR3) & ~ETR3_CF9GR);

	/* Set up generic decode ranges */
	if (!dev)
		return;
	if (dev->chip_info)
		config = dev->chip_info;
	if (!config)
		return;

	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, config->gen1_dec);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN2_DEC, config->gen2_dec);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN3_DEC, config->gen3_dec);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN4_DEC, config->gen4_dec);
}

static void early_gpio_init(void)
{
	pci_write_config32(PCH_LPC_DEV, GPIO_BASE, DEFAULT_GPIOBASE | 1);
	pci_write_config8(PCH_LPC_DEV, GPIO_CNTL, 0x10);

	setup_pch_gpios(&mainboard_gpio_map);
}

static void pch_default_disable(void)
{
	/* Must set BIT0 (hides performance counters PCI device).
	   coreboot enables the Rate Matching Hub which makes the UHCI PCI
	   devices disappear, so BIT5-12 and BIT28 can be set to hide those. */
	RCBA32(FD) = (1 << 28) | (0xff << 5) | 1;

	/* Set reserved bit to 1 */
	RCBA32(FD2) = 1;
}

void early_pch_init(void)
{
	early_lpc_init();
	mainboard_lpc_init();
	early_gpio_init();
	/* TODO, make this configurable */
	pch_setup_cir(NEHALEM_MOBILE);
	southbridge_configure_default_intmap();
	pch_default_disable();
	early_usb_init(mainboard_usb_ports);
}
