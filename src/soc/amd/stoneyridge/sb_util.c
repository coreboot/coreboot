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
