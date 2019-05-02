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
#include <amdblocks/acpimmio.h>
#include <soc/southbridge.h>

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
