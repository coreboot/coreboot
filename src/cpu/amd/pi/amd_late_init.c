/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#include <arch/acpi.h>
#include <bootstate.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>

#include <northbridge/amd/pi/agesawrapper.h>
#include <northbridge/amd/pi/agesawrapper_call.h>

static void agesawrapper_post_device(void *unused)
{
	if (acpi_is_wakeup_s3())
		return;

	AGESAWRAPPER(amdinitlate);

#if (1) /* NORTHBRIDGE_00730F01 */
	device_t dev;
	u32 value;
	dev = dev_find_slot(0, PCI_DEVFN(0, 0)); /* clear IoapicSbFeatureEn */
	pci_write_config32(dev, 0xF8, 0);
	pci_write_config32(dev, 0xFC, 5); /* TODO: move it to dsdt.asl */

	/* disable No Snoop */
	dev = dev_find_slot(0, PCI_DEVFN(1, 1));
	value = pci_read_config32(dev, 0x60);
	value &= ~(1 << 11);
	pci_write_config32(dev, 0x60, value);
#endif

	if (!acpi_s3_resume_allowed())
		return;

	AGESAWRAPPER(amdS3Save);
}

BOOT_STATE_INIT_ENTRIES(agesa_bscb) = {
	BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT,
	                      agesawrapper_post_device, NULL),
};
