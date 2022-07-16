/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <amdblocks/biosram.h>
#include <device/mmio.h>
#include <stdint.h>

/* BiosRam Ranges at 0xfed80500 or I/O 0xcd4/0xcd5 */
#define BIOSRAM_AP_ENTRY		0xe8 /* 8 bytes */
#define BIOSRAM_CBMEM_TOP		0xf0 /* 4 bytes */
#define BIOSRAM_UMA_SIZE		0xf4 /* 4 bytes */
#define BIOSRAM_UMA_BASE		0xf8 /* 8 bytes */

static uint8_t biosram_read8(uint8_t reg)
{
	return read8(acpimmio_biosram + reg);
}

static void biosram_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_biosram + reg, value);
}

static uint16_t biosram_read16(uint8_t reg) /* Must be 1 byte at a time */
{
	return (biosram_read8(reg + sizeof(uint8_t)) << 8 | biosram_read8(reg));
}

static void biosram_write16(uint8_t reg, uint16_t value)
{
	biosram_write8(reg, value & 0xff);
	value >>= 8;
	biosram_write8(reg + sizeof(uint8_t), value & 0xff);
}

static uint32_t biosram_read32(uint8_t reg)
{
	uint32_t value = biosram_read16(reg + sizeof(uint16_t)) << 16;
	return value | biosram_read16(reg);
}

static void biosram_write32(uint8_t reg, uint32_t value)
{
	biosram_write16(reg, value & 0xffff);
	value >>= 16;
	biosram_write16(reg + sizeof(uint16_t), value & 0xffff);
}

/* Access to BIOSRAM is only allowed through the abstractions below. */

void *get_ap_entry_ptr(void)
{
	return (void *)biosram_read32(BIOSRAM_AP_ENTRY);
}

void set_ap_entry_ptr(void *entry)
{
	biosram_write32(BIOSRAM_AP_ENTRY, (uintptr_t)entry);
}

void backup_top_of_low_cacheable(uintptr_t ramtop)
{
	biosram_write32(BIOSRAM_CBMEM_TOP, ramtop);
}

uintptr_t restore_top_of_low_cacheable(void)
{
	return biosram_read32(BIOSRAM_CBMEM_TOP);
}

void save_uma_size(uint32_t size)
{
	biosram_write32(BIOSRAM_UMA_SIZE, size);
}

void save_uma_base(uint64_t base)
{
	biosram_write32(BIOSRAM_UMA_BASE, (uint32_t)base);
	biosram_write32(BIOSRAM_UMA_BASE + 4, (uint32_t)(base >> 32));
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
