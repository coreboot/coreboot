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

#include <arch/acpi.h>
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

void misc_write32(u8 reg, u32 value)
{
	write32((void *)(MISC_MMIO_BASE + reg), value);
}

u32 misc_read32(u8 reg)
{
	return read32((void *)(MISC_MMIO_BASE + reg));
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

uint8_t biosram_read8(uint8_t offset)
{
	return read8((void *)(BIOSRAM_MMIO_BASE + offset));
}

void biosram_write8(uint8_t offset, uint8_t value)
{
	write8((void *)(BIOSRAM_MMIO_BASE + offset), value);
}

/* BiosRam may only be accessed a byte at a time */
uint16_t biosram_read16(uint8_t offset)
{
	int i;
	uint16_t value = 0;
	for (i = sizeof(value) - 1 ; i >= 0 ; i--)
		value = (value << 8) | biosram_read8(offset + i);
	return value;
}

uint32_t biosram_read32(uint8_t offset)
{
	uint32_t value = biosram_read16(offset + sizeof(uint16_t)) << 16;
	return value | biosram_read16(offset);
}

void biosram_write16(uint8_t offset, uint16_t value)
{
	int i;
	for (i = 0 ; i < sizeof(value) ; i++) {
		biosram_write8(offset + i, value & 0xff);
		value >>= 8;
	}
}

void biosram_write32(uint8_t offset, uint32_t value)
{
	int i;
	for (i = 0 ; i < sizeof(value) ; i++) {
		biosram_write8(offset + i, value & 0xff);
		value >>= 8;
	}
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

int acpi_get_sleep_type(void)
{
	return acpi_sleep_from_pm1(inw(pm_acpi_pm_cnt_blk()));
}

void save_uma_size(uint32_t size)
{
	biosram_write32(BIOSRAM_UMA_SIZE, size);
}

void save_uma_base(uint64_t base)
{
	biosram_write32(BIOSRAM_UMA_BASE, (uint32_t) base);
	biosram_write32(BIOSRAM_UMA_BASE + 4, (uint32_t) (base >> 32));
}

uint32_t get_uma_size(void)
{
	return biosram_read32(BIOSRAM_UMA_SIZE);
}

uint64_t get_uma_base(void)
{
	uint64_t base;
	base = biosram_read32(BIOSRAM_UMA_BASE);
	base |= ((uint64_t)(biosram_read32(BIOSRAM_UMA_BASE + 4)) << 32);
	return base;
}
