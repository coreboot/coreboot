/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <intelblocks/ioc.h>
#include <soc/iomap.h>

void ioc_reg_write32(uint32_t offset, uint32_t value)
{
	write32p(MCH_BASE_ADDRESS + offset, value);
}

uint32_t ioc_reg_read32(uint32_t offset)
{
	return read32p(MCH_BASE_ADDRESS + offset);
}

void ioc_reg_or32(uint32_t offset, uint32_t ordata)
{
	uint32_t data32;

	data32 = read32p(MCH_BASE_ADDRESS + offset);
	data32 |= ordata;
	write32p(MCH_BASE_ADDRESS + offset, data32);
}
