/*
 * This file is part of the coreboot project.
 *
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
 */

#include <stdint.h>
#include <arch/bootblock.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <device/pci_type.h>
#include "i82371eb.h"

#define PCI_ID(VENDOR_ID, DEVICE_ID) \
	((((DEVICE_ID) & 0xFFFF) << 16) | ((VENDOR_ID) & 0xFFFF))

static pci_devfn_t pci_locate_device(unsigned int pci_id, pci_devfn_t dev)
{
	for (; dev <= PCI_DEV(255, 31, 7); dev += PCI_DEV(0, 0, 1)) {
		unsigned int id;
		id = pci_read_config32(dev, 0);
		if (id == pci_id)
			return dev;
	}
	return PCI_DEV_INVALID;
}

/* TODO: Does not need to happen before console init. */
/* The whole rom is not accessible before this so limit
   the bootblock size. */
#if CONFIG_C_ENV_BOOTBLOCK_SIZE > 0x10000
#error "CONFIG_C_ENV_BOOTBLOCK_SIZE needs to be below 64KiB"
#endif
void bootblock_early_southbridge_init(void)
{
	u16 reg16;
	pci_devfn_t dev;

	/*
	 * Note: The Intel 82371AB/EB/MB ISA device can be on different
	 * PCI bus:device.function locations on different boards.
	 * Examples we encountered: 00:07.0, 00:04.0, or 00:14.0.
	 * But scanning for the PCI IDs (instead of hardcoding
	 * bus/device/function numbers) works on all boards.
	 */
	dev = pci_locate_device(PCI_ID(PCI_VENDOR_ID_INTEL,
				       PCI_DEVICE_ID_INTEL_82371AB_ISA), 0);

	/* Enable access to the whole ROM, disable ROM write access. */
	reg16 = pci_read_config16(dev, XBCS);
	reg16 |= LOWER_BIOS_ENABLE | EXT_BIOS_ENABLE | EXT_BIOS_ENABLE_1MB;
	reg16 &= ~(WRITE_PROTECT_ENABLE);	/* Disable ROM write access. */
	pci_write_config16(dev, XBCS, reg16);

	/* Enable (RTC and) upper NVRAM bank. */
	pci_write_config8(dev, RTCCFG, RTC_POS_DECODE | UPPER_RAM_EN | RTC_ENABLE);
}
