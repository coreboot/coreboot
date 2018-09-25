/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Siemens AG
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

#include <bootstate.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <hwilib.h>
#include <intelblocks/lpc_lib.h>
#include <timer.h>
#include <timestamp.h>
#include <baseboard/variants.h>

void variant_mainboard_final(void)
{
	struct device *dev;
	uint16_t cmd = 0;

	/* Set Master Enable for on-board PCI device. */
	dev = dev_find_device(PCI_VENDOR_ID_SIEMENS, 0x403e, 0);
	if (dev) {
		cmd = pci_read_config16(dev, PCI_COMMAND);
		cmd |= PCI_COMMAND_MASTER;
		pci_write_config16(dev, PCI_COMMAND, cmd);
	}
}

static void wait_for_legacy_dev(void *unused)
{
	uint32_t legacy_delay, us_since_boot;
	struct stopwatch sw;

	/* Open main hwinfo block. */
	if (hwilib_find_blocks("hwinfo.hex") != CB_SUCCESS)
		return;

	/* Get legacy delay parameter from hwinfo. */
	if (hwilib_get_field(LegacyDelay, (uint8_t *) &legacy_delay,
			      sizeof(legacy_delay)) != sizeof(legacy_delay))
		return;

	us_since_boot = get_us_since_boot();
	/* No need to wait if the time since boot is already long enough.*/
	if (us_since_boot > legacy_delay)
		return;
	stopwatch_init_msecs_expire(&sw, (legacy_delay - us_since_boot) / 1000);
	printk(BIOS_NOTICE, "Wait remaining %d of %d us for legacy devices...",
			legacy_delay - us_since_boot, legacy_delay);
	stopwatch_wait_until_expired(&sw);
	printk(BIOS_NOTICE, "done!\n");
}

BOOT_STATE_INIT_ENTRY(BS_DEV_ENUMERATE, BS_ON_ENTRY, wait_for_legacy_dev, NULL);
