/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
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
#include <assert.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>

#if !defined(CONFIG_PCR_BASE_ADDRESS) || (CONFIG_PCR_BASE_ADDRESS == 0)
#error "PCR_BASE_ADDRESS need to be non-zero!"
#endif

static void *__pcr_reg_address(uint8_t pid, uint16_t offset)
{
	uintptr_t reg_addr;

	/* Create an address based off of port id and offset. */
	reg_addr = CONFIG_PCR_BASE_ADDRESS;
	reg_addr += ((uintptr_t)pid) << PCR_PORTID_SHIFT;
	reg_addr += (uintptr_t)offset;

	return (void *)reg_addr;
}

void *pcr_reg_address(uint8_t pid, uint16_t offset)
{
	if (IS_ENABLED(CONFIG_PCR_COMMON_IOSF_1_0))
		assert(IS_ALIGNED(offset, sizeof(uint32_t)));

	return __pcr_reg_address(pid, offset);
}

/*
 * The mapping of addresses via the SBREG_BAR assumes the IOSF-SB
 * agents are using 32-bit aligned accesses for their configuration
 * registers. For IOSF versions greater than 1_0, IOSF-SB
 * agents can use any access (8/16/32 bit aligned) for their
 * configuration registers
 */
static inline void check_pcr_offset_align(uint16_t offset, size_t size)
{
	const size_t align = IS_ENABLED(CONFIG_PCR_COMMON_IOSF_1_0) ?
					sizeof(uint32_t) : size;

	assert(IS_ALIGNED(offset, align));
}

uint32_t pcr_read32(uint8_t pid, uint16_t offset)
{
	/* Ensure the PCR offset is correctly aligned. */
	assert(IS_ALIGNED(offset, sizeof(uint32_t)));

	return read32(__pcr_reg_address(pid, offset));
}

uint16_t pcr_read16(uint8_t pid, uint16_t offset)
{
	/* Ensure the PCR offset is correctly aligned. */
	check_pcr_offset_align(offset, sizeof(uint16_t));

	return read16(__pcr_reg_address(pid, offset));
}

uint8_t pcr_read8(uint8_t pid, uint16_t offset)
{
	/* Ensure the PCR offset is correctly aligned. */
	check_pcr_offset_align(offset, sizeof(uint8_t));

	return read8(__pcr_reg_address(pid, offset));
}

/*
 * After every write one needs to perform a read an innocuous register to
 * ensure the writes are completed for certain ports. This is done for
 * all ports so that the callers don't need the per-port knowledge for
 * each transaction.
 */
static inline void write_completion(uint8_t pid, uint16_t offset)
{
	read32(__pcr_reg_address(pid, ALIGN_DOWN(offset, sizeof(uint32_t))));
}

void pcr_write32(uint8_t pid, uint16_t offset, uint32_t indata)
{
	/* Ensure the PCR offset is correctly aligned. */
	assert(IS_ALIGNED(offset, sizeof(indata)));

	write32(__pcr_reg_address(pid, offset), indata);
	/* Ensure the writes complete. */
	write_completion(pid, offset);
}

void pcr_write16(uint8_t pid, uint16_t offset, uint16_t indata)
{
	/* Ensure the PCR offset is correctly aligned. */
	check_pcr_offset_align(offset, sizeof(uint16_t));

	write16(__pcr_reg_address(pid, offset), indata);
	/* Ensure the writes complete. */
	write_completion(pid, offset);
}

void pcr_write8(uint8_t pid, uint16_t offset, uint8_t indata)
{
	/* Ensure the PCR offset is correctly aligned. */
	check_pcr_offset_align(offset, sizeof(uint8_t));

	write8(__pcr_reg_address(pid, offset), indata);
	/* Ensure the writes complete. */
	write_completion(pid, offset);
}

void pcr_rmw32(uint8_t pid, uint16_t offset, uint32_t anddata, uint32_t ordata)
{
	uint32_t data32;

	data32 = pcr_read32(pid, offset);
	data32 &= anddata;
	data32 |= ordata;
	pcr_write32(pid, offset, data32);
}

void pcr_rmw16(uint8_t pid, uint16_t offset, uint16_t anddata, uint16_t ordata)
{
	uint16_t data16;

	data16 = pcr_read16(pid, offset);
	data16 &= anddata;
	data16 |= ordata;
	pcr_write16(pid, offset, data16);
}

void pcr_rmw8(uint8_t pid, uint16_t offset, uint8_t anddata, uint8_t ordata)
{
	uint8_t data8;

	data8 = pcr_read8(pid, offset);
	data8 &= anddata;
	data8 |= ordata;
	pcr_write8(pid, offset, data8);
}

void pcr_or32(uint8_t pid, uint16_t offset, uint32_t ordata)
{
	uint32_t data32;

	data32 = pcr_read32(pid, offset);
	data32 |= ordata;
	pcr_write32(pid, offset, data32);
}

void pcr_or16(uint8_t pid, uint16_t offset, uint16_t ordata)
{
	uint16_t data16;

	data16 = pcr_read16(pid, offset);
	data16 |= ordata;
	pcr_write16(pid, offset, data16);
}

void pcr_or8(uint8_t pid, uint16_t offset, uint8_t ordata)
{
	uint8_t data8;

	data8 = pcr_read8(pid, offset);
	data8 |= ordata;
	pcr_write8(pid, offset, data8);
}
