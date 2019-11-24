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
#include <amdblocks/acpimmio_map.h>
#include <amdblocks/acpimmio.h>

void enable_acpimmio_decode_pm24(void)
{
	uint32_t dw;

	dw = pm_io_read32(ACPIMMIO_DECODE_REGISTER_24);
	dw |= PM_24_ACPIMMIO_DECODE_EN;
	pm_io_write32(ACPIMMIO_DECODE_REGISTER_24, dw);
}

void enable_acpimmio_decode_pm04(void)
{
	uint32_t dw;

	dw = pm_io_read32(ACPIMMIO_DECODE_REGISTER_04);
	dw |= PM_04_ACPIMMIO_DECODE_EN;
	pm_io_write32(ACPIMMIO_DECODE_REGISTER_04, dw);
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

uint16_t biosram_read16(uint8_t reg) /* Must be 1 byte at a time */
{
	return (biosram_read8(reg + sizeof(uint8_t)) << 8 | biosram_read8(reg));
}

uint32_t biosram_read32(uint8_t reg)
{
	uint32_t value = biosram_read16(reg + sizeof(uint16_t)) << 16;
	return value | biosram_read16(reg);
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
