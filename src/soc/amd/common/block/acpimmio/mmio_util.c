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

u8 sm_pci_read8(u8 reg)
{
	return read8((void *)(ACPIMMIO_SM_PCI_BASE + reg));
}

u16 sm_pci_read16(u8 reg)
{
	return read16((void *)(ACPIMMIO_SM_PCI_BASE + reg));
}

u32 sm_pci_read32(u8 reg)
{
	return read32((void *)(ACPIMMIO_SM_PCI_BASE + reg));
}

void sm_pci_write8(u8 reg, u8 value)
{
	write8((void *)(ACPIMMIO_SM_PCI_BASE + reg), value);
}

void sm_pci_write16(u8 reg, u16 value)
{
	write16((void *)(ACPIMMIO_SM_PCI_BASE + reg), value);
}

void sm_pci_write32(u8 reg, u32 value)
{
	write32((void *)(ACPIMMIO_SM_PCI_BASE + reg), value);
}

uint8_t smi_read8(uint8_t reg)
{
	return read8((void *)(ACPIMMIO_SMI_BASE + reg));
}

uint16_t smi_read16(uint8_t reg)
{
	return read16((void *)(ACPIMMIO_SMI_BASE + reg));
}

uint32_t smi_read32(uint8_t reg)
{
	return read32((void *)(ACPIMMIO_SMI_BASE + reg));
}

void smi_write8(uint8_t reg, uint8_t value)
{
	write8((void *)(ACPIMMIO_SMI_BASE + reg), value);
}

void smi_write16(uint8_t reg, uint16_t value)
{
	write16((void *)(ACPIMMIO_SMI_BASE + reg), value);
}

void smi_write32(uint8_t reg, uint32_t value)
{
	write32((void *)(ACPIMMIO_SMI_BASE + reg), value);
}

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

uint8_t biosram_read8(uint8_t reg)
{
	return read8((void *)(ACPIMMIO_BIOSRAM_BASE + reg));
}

uint16_t biosram_read16(uint8_t reg) /* Must be 1 byte at a time */
{
	return (biosram_read8(reg + sizeof(uint8_t)) << 8 | biosram_read8(reg));
}

uint32_t biosram_read32(uint8_t reg)
{
	uint32_t value = biosram_read16(reg + sizeof(uint16_t)) << 16;
	return value | biosram_read16(reg);
}

void biosram_write8(uint8_t reg, uint8_t value)
{
	write8((void *)(ACPIMMIO_BIOSRAM_BASE + reg), value);
}

void biosram_write16(uint8_t reg, uint16_t value)
{
	biosram_write8(reg, value & 0xff);
	value >>= 8;
	biosram_write8(reg + sizeof(uint8_t), value & 0xff);
}

void biosram_write32(uint8_t reg, uint32_t value)
{
	biosram_write16(reg, value & 0xffff);
	value >>= 16;
	biosram_write16(reg + sizeof(uint16_t), value & 0xffff);
}

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

u8 aoac_read8(u8 reg)
{
	return read8((void *)(ACPIMMIO_AOAC_BASE + reg));
}

void aoac_write8(u8 reg, u8 value)
{
	write8((void *)(ACPIMMIO_AOAC_BASE + reg), value);
}
