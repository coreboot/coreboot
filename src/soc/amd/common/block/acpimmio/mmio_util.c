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

#include <types.h>
#include <arch/io.h>
#include <device/mmio.h>
#include <amdblocks/acpimmio_map.h>
#include <amdblocks/acpimmio.h>

void enable_acpimmio_decode(void)
{
	uint32_t dw;

	dw = pm_io_read32(ACPIMMIO_DECODE_REGISTER);
	dw |= ACPIMMIO_DECODE_EN;
	pm_io_write32(ACPIMMIO_DECODE_REGISTER, dw);
}

/* PM registers are accessed a byte at a time via CD6/CD7 */
uint8_t pm_io_read8(uint8_t reg)
{
	outb(reg, PM_INDEX);
	return inb(PM_DATA);
}

uint16_t pm_io_read16(uint8_t reg)
{
	return (pm_io_read8(reg + sizeof(uint8_t)) << 8) | pm_io_read8(reg);
}

uint32_t pm_io_read32(uint8_t reg)
{
	return (pm_io_read16(reg + sizeof(uint16_t)) << 16) | pm_io_read16(reg);
}

void pm_io_write8(uint8_t reg, uint8_t value)
{
	outb(reg, PM_INDEX);
	outb(value, PM_DATA);
}

void pm_io_write16(uint8_t reg, uint16_t value)
{
	pm_io_write8(reg, value & 0xff);
	value >>= 8;
	pm_io_write8(reg + sizeof(uint8_t), value & 0xff);
}

void pm_io_write32(uint8_t reg, uint32_t value)
{
	pm_io_write16(reg, value & 0xffff);
	value >>= 16;
	pm_io_write16(reg + sizeof(uint16_t), value & 0xffff);
}

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

/* asf read/write - access registers at 0xfed80900 - not currently used */

u8 asf_read8(u8 reg)
{
	return read8((void *)(ACPIMMIO_ASF_BASE + reg));
}

u16 asf_read16(u8 reg)
{
	return read16((void *)(ACPIMMIO_ASF_BASE + reg));
}

void asf_write8(u8 reg, u8 value)
{
	write8((void *)(ACPIMMIO_ASF_BASE + reg), value);
}

void asf_write16(u8 reg, u16 value)
{
	write16((void *)(ACPIMMIO_ASF_BASE + reg), value);
}

/* smbus read/write - access registers at 0xfed80a00 and ASF at 0xfed80900 */

u8 smbus_read8(u8 reg)
{
	return read8((void *)(ACPIMMIO_SMBUS_BASE + reg));
}

u16 smbus_read16(u8 reg)
{
	return read16((void *)(ACPIMMIO_SMBUS_BASE + reg));
}

void smbus_write8(u8 reg, u8 value)
{
	write8((void *)(ACPIMMIO_SMBUS_BASE + reg), value);
}

void smbus_write16(u8 reg, u16 value)
{
	write16((void *)(ACPIMMIO_SMBUS_BASE + reg), value);
}

/* wdt read/write - access registers at 0xfed80b00 - not currently used */

/* hpet read/write - access registers at 0xfed80c00 - not currently used */

/* iomux read/write - access registers at 0xfed80d00 */

u8 iomux_read8(u8 reg)
{
	return read8((void *)(ACPIMMIO_IOMUX_BASE + reg));
}

u16 iomux_read16(u8 reg)
{
	return read16((void *)(ACPIMMIO_IOMUX_BASE + reg));
}

u32 iomux_read32(u8 reg)
{
	return read32((void *)(ACPIMMIO_IOMUX_BASE + reg));
}

void iomux_write8(u8 reg, u8 value)
{
	write8((void *)(ACPIMMIO_IOMUX_BASE + reg), value);
}

void iomux_write16(u8 reg, u16 value)
{
	write16((void *)(ACPIMMIO_IOMUX_BASE + reg), value);
}

void iomux_write32(u8 reg, u32 value)
{
	write32((void *)(ACPIMMIO_IOMUX_BASE + reg), value);
}

/* misc read/write - access registers at 0xfed80e00 */

u8 misc_read8(u8 reg)
{
	return read8((void *)(ACPIMMIO_MISC_BASE + reg));
}

u16 misc_read16(u8 reg)
{
	return read16((void *)(ACPIMMIO_MISC_BASE + reg));
}

u32 misc_read32(u8 reg)
{
	return read32((void *)(ACPIMMIO_MISC_BASE + reg));
}

void misc_write8(u8 reg, u8 value)
{
	write8((void *)(ACPIMMIO_MISC_BASE + reg), value);
}

void misc_write16(u8 reg, u16 value)
{
	write16((void *)(ACPIMMIO_MISC_BASE + reg), value);
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

u8 aoac_read8(u8 reg)
{
	return read8((void *)(ACPIMMIO_AOAC_BASE + reg));
}

void aoac_write8(u8 reg, u8 value)
{
	write8((void *)(ACPIMMIO_AOAC_BASE + reg), value);
}
