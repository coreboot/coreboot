/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <arch/io.h>
#include <amdblocks/acpimmio.h>

/* IO index/data for accessing PMIO prior to enabling MMIO decode */
#define PM_INDEX			0xcd6
#define PM_DATA				0xcd7

void enable_acpimmio_decode_pm04(void)
{
	uint32_t dw;

	dw = pm_io_read32(ACPIMMIO_DECODE_REGISTER_04);
	dw |= PM_04_ACPIMMIO_DECODE_EN;
	pm_io_write32(ACPIMMIO_DECODE_REGISTER_04, dw);
}

void fch_io_enable_legacy_io(void)
{
	pm_io_write32(PM_DECODE_EN, pm_io_read32(PM_DECODE_EN) | LEGACY_IO_EN);
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
