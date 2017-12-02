/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Advanced Micro Devices, Inc.
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

#include <soc/southbridge.h>

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

u16 pm_read16(u8 reg)
{
	return read16((void *)(PM_MMIO_BASE + reg));
}

void pm_write32(u8 reg, u32 value)
{
	write32((void *)(PM_MMIO_BASE + reg), value);
}

u32 pm_read32(u8 reg)
{
	return read32((void *)(PM_MMIO_BASE + reg));
}

void smi_write32(uint8_t offset, uint32_t value)
{
	write32((void *)(APU_SMI_BASE + offset), value);
}

uint32_t smi_read32(uint8_t offset)
{
	return read32((void *)(APU_SMI_BASE + offset));
}

uint16_t smi_read16(uint8_t offset)
{
	return read16((void *)(APU_SMI_BASE + offset));
}

void smi_write16(uint8_t offset, uint16_t value)
{
	write16((void *)(APU_SMI_BASE + offset), value);
}

uint8_t smi_read8(uint8_t offset)
{
	return read8((void *)(APU_SMI_BASE + offset));
}

void smi_write8(uint8_t offset, uint8_t value)
{
	write8((void *)(APU_SMI_BASE + offset), value);
}

uint16_t pm_acpi_pm_cnt_blk(void)
{
	return pm_read16(PM1_CNT_BLK);
}

uint16_t pm_acpi_pm_evt_blk(void)
{
	return pm_read16(PM_EVT_BLK);
}

void xhci_pm_write8(uint8_t reg, uint8_t value)
{
	write8((void *)(XHCI_ACPI_PM_MMIO_BASE + reg), value);
}

uint8_t xhci_pm_read8(uint8_t reg)
{
	return read8((void *)(XHCI_ACPI_PM_MMIO_BASE + reg));
}

void xhci_pm_write16(uint8_t reg, uint16_t value)
{
	write16((void *)(XHCI_ACPI_PM_MMIO_BASE + reg), value);
}

uint16_t xhci_pm_read16(uint8_t reg)
{
	return read16((void *)(XHCI_ACPI_PM_MMIO_BASE + reg));
}

void xhci_pm_write32(uint8_t reg, uint32_t value)
{
	write32((void *)(XHCI_ACPI_PM_MMIO_BASE + reg), value);
}

uint32_t xhci_pm_read32(uint8_t reg)
{
	return read32((void *)(XHCI_ACPI_PM_MMIO_BASE + reg));
}
