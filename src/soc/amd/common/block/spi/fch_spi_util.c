/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/lpc.h>
#include <amdblocks/spi.h>
#include <arch/mmio.h>
#include <assert.h>
#include <stdint.h>

static uintptr_t spi_base;

void spi_set_base(void *base)
{
	spi_base = (uintptr_t)base;
}

uintptr_t spi_get_bar(void)
{
	if (ENV_X86 && !spi_base)
		spi_set_base((void *)lpc_get_spibase());
	ASSERT(spi_base);

	return spi_base;
}

uint8_t spi_read8(uint8_t reg)
{
	return read8((void *)(spi_get_bar() + reg));
}

uint16_t spi_read16(uint8_t reg)
{
	return read16((void *)(spi_get_bar() + reg));
}

uint32_t spi_read32(uint8_t reg)
{
	return read32((void *)(spi_get_bar() + reg));
}

void spi_write8(uint8_t reg, uint8_t val)
{
	write8((void *)(spi_get_bar() + reg), val);
}

void spi_write16(uint8_t reg, uint16_t val)
{
	write16((void *)(spi_get_bar() + reg), val);
}

void spi_write32(uint8_t reg, uint32_t val)
{
	write32((void *)(spi_get_bar() + reg), val);
}
