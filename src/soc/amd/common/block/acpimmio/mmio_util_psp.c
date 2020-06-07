/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <arch/io.h>
#include <device/mmio.h>
#include <amdblocks/acpimmio.h>

static uintptr_t iomux_bar;

void iomux_set_bar(void *bar)
{
	iomux_bar = (uintptr_t)bar;
}

u8 iomux_read8(u8 reg)
{
	return read8((void *)(iomux_bar + reg));
}

u16 iomux_read16(u8 reg)
{
	return read16((void *)(iomux_bar + reg));
}

u32 iomux_read32(u8 reg)
{
	return read32((void *)(iomux_bar + reg));
}

void iomux_write8(u8 reg, u8 value)
{
	write8((void *)(iomux_bar + reg), value);
}

void iomux_write16(u8 reg, u16 value)
{
	write16((void *)(iomux_bar + reg), value);
}

void iomux_write32(u8 reg, u32 value)
{
	write32((void *)(iomux_bar + reg), value);
}

static uintptr_t misc_bar;

void misc_set_bar(void *bar)
{
	misc_bar = (uintptr_t)bar;
}

u8 misc_read8(u8 reg)
{
	return read8((void *)(misc_bar + reg));
}

u16 misc_read16(u8 reg)
{
	return read16((void *)(misc_bar + reg));
}

u32 misc_read32(u8 reg)
{
	return read32((void *)(misc_bar + reg));
}

void misc_write8(u8 reg, u8 value)
{
	write8((void *)(misc_bar + reg), value);
}

void misc_write16(u8 reg, u16 value)
{
	write16((void *)(misc_bar + reg), value);
}

void misc_write32(u8 reg, u32 value)
{
	write32((void *)(misc_bar + reg), value);
}

static uintptr_t gpio_bar;

void gpio_set_bar(void *bar)
{
	gpio_bar = (uintptr_t)bar;
}

void *gpio_get_bar(void)
{
	return (void *)gpio_bar;
}

static uintptr_t aoac_bar;

void aoac_set_bar(void *bar)
{
	aoac_bar = (uintptr_t)bar;
}

u8 aoac_read8(u8 reg)
{
	return read8((void *)(aoac_bar + reg));
}

void aoac_write8(u8 reg, u8 value)
{
	write8((void *)(aoac_bar + reg), value);
}

static uintptr_t io_bar;

void io_set_bar(void *bar)
{
	io_bar = (uintptr_t)bar;
}

u8 io_read8(u16 reg)
{
	return read8((void *)(io_bar + reg));
}

void io_write8(u16 reg, u8 value)
{
	write8((void *)(io_bar + reg), value);
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
