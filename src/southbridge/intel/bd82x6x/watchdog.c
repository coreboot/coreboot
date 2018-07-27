/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
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

#include <console/console.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <southbridge/intel/common/pmbase.h>
#include <southbridge/intel/bd82x6x/pch.h>

#include <watchdog.h>

/*
 *  Disable PCH watchdog timer
 */
void watchdog_off(void)
{
	unsigned int value;
	struct device *dev;

	/* Get LPC device. */
	dev = dev_find_slot(0, PCI_DEVFN(0x1f, 0));

	/* Disable interrupt. */
	value = pci_read_config16(dev, PCI_COMMAND);
	value |= PCI_COMMAND_INT_DISABLE;
	pci_write_config16(dev, PCI_COMMAND, value);

	/* Disable the watchdog timer. */
	value = read_pmbase16(TCO1_CNT);
	value |= TCO_TMR_HLT;
	write_pmbase16(TCO1_CNT, value);

	/* Clear TCO timeout status. */
	write_pmbase16(TCO1_STS, TCO1_TIMEOUT);
	write_pmbase16(TCO2_STS, SECOND_TO_STS);

	/* FIXME: Set RCBA GCS Bit5 "No Reboot" ? */

	printk(BIOS_DEBUG, "PCH: watchdog disabled\n");
}
