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

#include <arch/io.h>
#include <device/mmio.h>
#include <arch/acpi.h>
#include <soc/southbridge.h>

/* smbus pci read/write - access registers at 0xfed80000 - currently unused */

/* smi read/write - access registers at 0xfed80200 */

uint8_t smi_read8(uint8_t offset)
{
	return read8((void *)(ACPIMMIO_SMI_BASE + offset));
}

uint16_t smi_read16(uint8_t offset)
{
	return read16((void *)(ACPIMMIO_SMI_BASE + offset));
}

uint32_t smi_read32(uint8_t offset)
{
	return read32((void *)(ACPIMMIO_SMI_BASE + offset));
}

void smi_write8(uint8_t offset, uint8_t value)
{
	write8((void *)(ACPIMMIO_SMI_BASE + offset), value);
}

void smi_write16(uint8_t offset, uint16_t value)
{
	write16((void *)(ACPIMMIO_SMI_BASE + offset), value);
}

void smi_write32(uint8_t offset, uint32_t value)
{
	write32((void *)(ACPIMMIO_SMI_BASE + offset), value);
}

/* pm read/write - access registers at 0xfed80300 */

u8 pm_read8(u8 reg)
{
	return read8((void *)(ACPIMMIO_PMIO_BASE + reg));
}

u16 pm_read16(u8 reg)
{
	return read16((void *)(ACPIMMIO_PMIO_BASE + reg));
}

u32 pm_read32(u8 reg)
{
	return read32((void *)(ACPIMMIO_PMIO_BASE + reg));
}

void pm_write8(u8 reg, u8 value)
{
	write8((void *)(ACPIMMIO_PMIO_BASE + reg), value);
}

void pm_write16(u8 reg, u16 value)
{
	write16((void *)(ACPIMMIO_PMIO_BASE + reg), value);
}

void pm_write32(u8 reg, u32 value)
{
	write32((void *)(ACPIMMIO_PMIO_BASE + reg), value);
}

/* pm2 read/write - access registers at 0xfed80400 - currently unused */

/* biosram read/write - access registers at 0xfed80500 */

uint8_t biosram_read8(uint8_t offset)
{
	return read8((void *)(ACPIMMIO_BIOSRAM_BASE + offset));
}

uint16_t biosram_read16(uint8_t offset) /* Must be 1 byte at a time */
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

void biosram_write8(uint8_t offset, uint8_t value)
{
	write8((void *)(ACPIMMIO_BIOSRAM_BASE + offset), value);
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

/* cmosram read/write - access registers at 0xfed80600 - currently unused */

/* cmos read/write - access registers at 0xfed80700 - currently unused */

/* acpi read/write - access registers at 0xfed80800 */

u8 acpi_read8(u8 reg)
{
	return read8((void *)(ACPIMMIO_ACPI_BASE + reg));
}

u16 acpi_read16(u8 reg)
{
	return read16((void *)(ACPIMMIO_ACPI_BASE + reg));
}

u32 acpi_read32(u8 reg)
{
	return read32((void *)(ACPIMMIO_ACPI_BASE + reg));
}

void acpi_write8(u8 reg, u8 value)
{
	write8((void *)(ACPIMMIO_ACPI_BASE + reg), value);
}

void acpi_write16(u8 reg, u16 value)
{
	write16((void *)(ACPIMMIO_ACPI_BASE + reg), value);
}

void acpi_write32(u8 reg, u32 value)
{
	write32((void *)(ACPIMMIO_ACPI_BASE + reg), value);
}

/* smbus read/write - access registers at 0xfed80a00 and ASF at 0xfed80900 */

void smbus_write8(uint32_t mmio, uint8_t reg, uint8_t value)
{
	write8((void *)(mmio + reg), value);
}

uint8_t smbus_read8(uint32_t mmio, uint8_t reg)
{
	return read8((void *)(mmio + reg));
}

/* wdt read/write - access registers at 0xfed80b00 - not currently used */

/* hpet read/write - access registers at 0xfed80c00 - not currently used */

/* iomux read/write - access registers at 0xfed80d00 - not currently used */

/* misc read/write - access registers at 0xfed80e00 */

u32 misc_read32(u8 reg)
{
	return read32((void *)(ACPIMMIO_MISC_BASE + reg));
}

void misc_write32(u8 reg, u32 value)
{
	write32((void *)(ACPIMMIO_MISC_BASE + reg), value);
}

/* dpvga read/write - access registers at 0xfed81400 - not currently used */

/* gpio bk 0 read/write - access registers at 0xfed81500 - not currently used */
/* gpio bk 1 read/write - access registers at 0xfed81600 - not currently used */
/* gpio bk 2 read/write - access registers at 0xfed81700 - not currently used */

/* xhci_pm read/write - access registers at 0xfed81c00 */

uint8_t xhci_pm_read8(uint8_t reg)
{
	return read8((void *)(ACPIMMIO_XHCIPM_BASE + reg));
}

uint16_t xhci_pm_read16(uint8_t reg)
{
	return read16((void *)(ACPIMMIO_XHCIPM_BASE + reg));
}

uint32_t xhci_pm_read32(uint8_t reg)
{
	return read32((void *)(ACPIMMIO_XHCIPM_BASE + reg));
}

void xhci_pm_write8(uint8_t reg, uint8_t value)
{
	write8((void *)(ACPIMMIO_XHCIPM_BASE + reg), value);
}

void xhci_pm_write16(uint8_t reg, uint16_t value)
{
	write16((void *)(ACPIMMIO_XHCIPM_BASE + reg), value);
}

void xhci_pm_write32(uint8_t reg, uint32_t value)
{
	write32((void *)(ACPIMMIO_XHCIPM_BASE + reg), value);
}

/* acdc_tmr read/write - access registers at 0xfed81d00 */

/* aoac read/write - access registers at 0xfed81e00 - not currently used */

uint16_t pm_acpi_pm_cnt_blk(void)
{
	return pm_read16(PM1_CNT_BLK);
}

uint16_t pm_acpi_pm_evt_blk(void)
{
	return pm_read16(PM_EVT_BLK);
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
