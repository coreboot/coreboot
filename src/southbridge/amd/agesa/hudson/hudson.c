/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

#include <console/console.h>

#include <arch/io.h>
#include <arch/acpi.h>

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cbmem.h>
#include "hudson.h"
#include "imc.h"
#include "smbus.h"
#include "smi.h"

/* Offsets from ACPI_MMIO_BASE
 * This is defined by AGESA, but we don't include AGESA headers to avoid
 * polluting the namespace.
 */
#define PM_MMIO_BASE 0xfed80300

void pm_write8(u8 reg, u8 value)
{
	write8((void *)((uintptr_t)PM_MMIO_BASE + reg), value);
}

u8 pm_read8(u8 reg)
{
	return read8((void *)((uintptr_t)PM_MMIO_BASE + reg));
}

void pm_write16(u8 reg, u16 value)
{
	write16((void *)((uintptr_t)PM_MMIO_BASE + reg), value);
}

u16 pm_read16(u16 reg)
{
	return read16((void *)((uintptr_t)PM_MMIO_BASE + reg));
}

#define PM_REG_USB_ENABLE	0xef

enum usb_enable {
	USB_EN_DEVFN_12_0 = (1 << 0),
	USB_EN_DEVFN_12_2 = (1 << 1),
	USB_EN_DEVFN_13_0 = (1 << 2),
	USB_EN_DEVFN_13_2 = (1 << 3),
	USB_EN_DEVFN_16_0 = (1 << 4),
	USB_EN_DEVFN_16_2 = (1 << 5),
};

static void hudson_disable_usb(u8 disable)
{
	u8 reg8;

	/* Bit 7 handles routing, 6 is reserved. we don't mess with those */
	disable &= 0x3f;

	reg8 = pm_read8(PM_REG_USB_ENABLE);
	reg8 &= ~disable;
	pm_write8(PM_REG_USB_ENABLE, reg8);
}

void hudson_enable(struct device *dev)
{
	printk(BIOS_DEBUG, "hudson_enable()\n");
	switch (dev->path.pci.devfn) {
	case PCI_DEVFN(0x14, 5):
		if (dev->enabled == 0) {
			u32 usb_device_id = pci_read_config32(dev, 0) >> 16;
			u8 reg8;
			if (usb_device_id == PCI_DEVICE_ID_AMD_SB900_USB_20_5) {
				/* turn off and remove device 0:14.5 from PCI space */
				reg8 = pm_read8(0xef);
				reg8 &= ~(1 << 6);
				pm_write8(0xef, reg8);
			}
		}
		break;

	case PCI_DEVFN(0x14, 7):
		if (dev->enabled == 0) {
			u32 sd_device_id = pci_read_config32(dev, 0) >> 16;
			/* turn off the SDHC controller in the PM reg */
			u8 reg8;
			if (sd_device_id == PCI_DEVICE_ID_AMD_HUDSON_SD) {
				reg8 = pm_read8(0xe7);
				reg8 &= ~(1 << 0);
				pm_write8(0xe7, reg8);
			}
			else if (sd_device_id == PCI_DEVICE_ID_AMD_YANGTZE_SD) {
				reg8 = pm_read8(0xe8);
				reg8 &= ~(1 << 0);
				pm_write8(0xe8, reg8);
			}
			/* remove device 0:14.7 from PCI space */
			reg8 = pm_read8(0xd3);
			reg8 &= ~(1 << 6);
			pm_write8(0xd3, reg8);
		}
		break;

	/* Make sure to disable other functions if function 0 is disabled */
	case PCI_DEVFN(0x12, 0):
		if (dev->enabled == 0)
			hudson_disable_usb(USB_EN_DEVFN_12_0);
	case PCI_DEVFN(0x12, 2):	/* Fall through */
		if (dev->enabled == 0)
			hudson_disable_usb(USB_EN_DEVFN_12_2);
		break;
	case PCI_DEVFN(0x13, 0):
		if (dev->enabled == 0)
			hudson_disable_usb(USB_EN_DEVFN_13_0);
	case PCI_DEVFN(0x13, 2):	/* Fall through */
		if (dev->enabled == 0)
			hudson_disable_usb(USB_EN_DEVFN_13_2);
		break;
	case PCI_DEVFN(0x16, 0):
		if (dev->enabled == 0)
			hudson_disable_usb(USB_EN_DEVFN_16_0);
	case PCI_DEVFN(0x16, 2):	/* Fall through */
		if (dev->enabled == 0)
			hudson_disable_usb(USB_EN_DEVFN_16_2);
		break;
	default:
		break;
	}
}


static void hudson_init_acpi_ports(void)
{
	/* We use some of these ports in SMM regardless of whether or not
	 * ACPI tables are generated. Enable these ports indiscriminately.
	 */

	pm_write16(0x60, ACPI_PM_EVT_BLK);
	pm_write16(0x62, ACPI_PM1_CNT_BLK);
	pm_write16(0x64, ACPI_PM_TMR_BLK);
	pm_write16(0x68, ACPI_GPE0_BLK);
	/* CpuControl is in \_PR.CP00, 6 bytes */
	pm_write16(0x66, ACPI_CPU_CONTROL);

	if (IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)) {
		pm_write16(0x6a, ACPI_SMI_CTL_PORT);
		hudson_enable_acpi_cmd_smi();
	} else {
		pm_write16(0x6a, 0);
	}

	/* AcpiDecodeEnable, When set, SB uses the contents of the PM registers
	 * at index 60-6B to decode ACPI I/O address. AcpiSmiEn & SmiCmdEn
	 */
	pm_write8(0x74, 1<<0 | 1<<1 | 1<<4 | 1<<2);
}

static void hudson_init(void *chip_info)
{
	hudson_init_acpi_ports();
}

static void hudson_final(void *chip_info)
{
	/* AMD AGESA does not enable thermal zone, so we enable it here. */
	if (IS_ENABLED(CONFIG_HUDSON_IMC_FWM) &&
			!IS_ENABLED(CONFIG_ACPI_ENABLE_THERMAL_ZONE))
		enable_imc_thermal_zone();
}

struct chip_operations southbridge_amd_agesa_hudson_ops = {
	CHIP_NAME("ATI HUDSON")
	.enable_dev = hudson_enable,
	.init = hudson_init,
	.final = hudson_final
};
