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
#include "smbus.h"
#include "smi.h"
#include "fchec.h"

int acpi_get_sleep_type(void)
{
	u16 tmp = inw(ACPI_PM1_CNT_BLK);
	tmp = ((tmp & (7 << 10)) >> 10);
	return (int)tmp;
}

void pm_write8(u8 reg, u8 value)
{
	write8((void *)(PM_MMIO_BASE + reg), value);
}

u8 pm_read8(u8 reg)
{
	return read8((void *)(PM_MMIO_BASE + reg));
}

void pm_write16(u8 reg, u16 value)
{
	write16((void *)(PM_MMIO_BASE + reg), value);
}

u16 pm_read16(u16 reg)
{
	return read16((void *)(PM_MMIO_BASE + reg));
}

void hudson_enable(struct device *dev)
{
	printk(BIOS_DEBUG, "hudson_enable()\n");
	switch (dev->path.pci.devfn) {
	case (0x14 << 3) | 7: /* 0:14.7  SD */
		if (dev->enabled == 0) {
			// read the VENDEV ID
			struct device *sd_dev =
				dev_find_slot( 0, PCI_DEVFN( 0x14, 7));
			u32 sd_device_id = pci_read_config32( sd_dev, 0) >> 16;
			/* turn off the SDHC controller in the PM reg */
			u8 reg8;
			if (sd_device_id == PCI_DEVICE_ID_AMD_HUDSON_SD) {
				reg8 = pm_read8(PM_HUD_SD_FLASH_CTRL);
				reg8 &= ~BIT(0);
				pm_write8(PM_HUD_SD_FLASH_CTRL, reg8);
			}
			else if (sd_device_id == PCI_DEVICE_ID_AMD_YANGTZE_SD) {
				reg8 = pm_read8(PM_YANG_SD_FLASH_CTRL);
				reg8 &= ~BIT(0);
				pm_write8(PM_YANG_SD_FLASH_CTRL, reg8);
			}
			/* remove device 0:14.7 from PCI space */
			reg8 = pm_read8(PM_MANUAL_RESET);
			reg8 &= ~BIT(6);
			pm_write8(PM_MANUAL_RESET, reg8);
		}
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

	pm_write16(PM_EVT_BLK, ACPI_PM_EVT_BLK);
	pm_write16(PM1_CNT_BLK, ACPI_PM1_CNT_BLK);
	pm_write16(PM_TMR_BLK, ACPI_PM_TMR_BLK);
	pm_write16(PM_GPE0_BLK, ACPI_GPE0_BLK);
	/* CpuControl is in \_PR.CP00, 6 bytes */
	pm_write16(PM_CPU_CTRL, ACPI_CPU_CONTROL);

	if (IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)) {
		pm_write16(PM_ACPI_SMI_CMD, ACPI_SMI_CTL_PORT);
		hudson_enable_acpi_cmd_smi();
	} else {
		pm_write16(PM_ACPI_SMI_CMD, 0);
	}

	/* AcpiDecodeEnable, When set, SB uses the contents of the PM registers
	 * at index 60-6B to decode ACPI I/O address. AcpiSmiEn & SmiCmdEn
	 */
	pm_write8(PM_ACPI_CONF, BIT(0) | BIT(1) | BIT(4) | BIT(2));
}

static void hudson_init(void *chip_info)
{
	hudson_init_acpi_ports();
}

static void hudson_final(void *chip_info)
{
	if (IS_ENABLED(CONFIG_HUDSON_IMC_FWM)) {
		agesawrapper_fchecfancontrolservice();
		if (!IS_ENABLED(CONFIG_ACPI_ENABLE_THERMAL_ZONE))
			enable_imc_thermal_zone();
	}
}

struct chip_operations southbridge_amd_pi_hudson_ops = {
	CHIP_NAME("ATI HUDSON")
	.enable_dev = hudson_enable,
	.init = hudson_init,
	.final = hudson_final
};
