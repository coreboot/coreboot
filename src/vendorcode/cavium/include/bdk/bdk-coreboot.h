/*
 * This file is part of the coreboot project.
 * Copyright (c) 2003-2017  Cavium Inc. (support@cavium.com). All rights
 * reserved.
 * Copyright 2018-present Facebook, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * access.h: Wrappers for memory access
 */

#ifndef __BDK_BDK_COREBOOT_H
#define __BDK_BDK_COREBOOT_H

#include <arch/io.h>
#include <delay.h>

/**
 * Convert a memory pointer (void*) into a hardware compatible
 * memory address (uint64_t). Cavium hardware widgets don't
 * understand logical addresses.
 *
 * @param ptr    C style memory pointer
 * @return Hardware physical address
 */
static inline uint64_t bdk_ptr_to_phys(void *ptr)
{
	/* PA = VA for coreboot's purposes */
	return (uint64_t)ptr;
}

/**
 * Convert a hardware physical address (uint64_t) into a
 * memory pointer (void *).
 *
 * @param physical_address
 *               Hardware physical address to memory
 * @return Pointer to memory
 */
static inline void *bdk_phys_to_ptr(uint64_t physical_address)
{
	/* PA = VA for coreboot's purposes */
	return (void *)physical_address;
}

static inline void bdk_write64_int64(uint64_t address, int64_t value)
{
	dmb();
	*(volatile int64_t *)address = value;
	dmb();
}

static inline void bdk_write64_uint64(uint64_t address, uint64_t value)
{
	write64(bdk_phys_to_ptr(address), value);
}

static inline void bdk_write64_int32(uint64_t address, int32_t value)
{
	dmb();
	*(volatile int32_t *)address = value;
	dmb();
}

static inline void bdk_write64_uint32(uint64_t address, uint32_t value)
{
	write32(bdk_phys_to_ptr(address), value);
}

static inline void bdk_write64_int16(uint64_t address, int16_t value)
{
	dmb();
	*(volatile int16_t *)address = value;
	dmb();
}

static inline void bdk_write64_uint16(uint64_t address, uint16_t value)
{
	write16(bdk_phys_to_ptr(address), value);
}

static inline void bdk_write64_int8(uint64_t address, int8_t value)
{
	dmb();
	*(volatile int8_t *)address = value;
	dmb();
}

static inline void bdk_write64_uint8(uint64_t address, uint8_t value)
{
	write8(bdk_phys_to_ptr(address), value);
}

static inline int64_t bdk_read64_int64(uint64_t address)
{
	return *(volatile int64_t *)bdk_phys_to_ptr(address);
}

static inline uint64_t bdk_read64_uint64(uint64_t address)
{
	return read64(bdk_phys_to_ptr(address));
}

static inline int32_t bdk_read64_int32(uint64_t address)
{
	return *(volatile int32_t *)bdk_phys_to_ptr(address);
}

static inline uint32_t bdk_read64_uint32(uint64_t address)
{
	return read32(bdk_phys_to_ptr(address));
}

static inline int16_t bdk_read64_int16(uint64_t address)
{
	return *(volatile int16_t *)bdk_phys_to_ptr(address);
}

static inline uint16_t bdk_read64_uint16(uint64_t address)
{
	return read16(bdk_phys_to_ptr(address));
}

static inline int8_t bdk_read64_int8(uint64_t address)
{
	return *(volatile int8_t *)bdk_phys_to_ptr(address);
}

static inline uint8_t bdk_read64_uint8(uint64_t address)
{
	return read8(bdk_phys_to_ptr(address));
}

/**
 * Returns the number of bits set in the provided value.
 * Simple wrapper for POP instruction.
 *
 * @param val    32 bit value to count set bits in
 *
 * @return Number of bits set
 */
static inline uint32_t bdk_pop(uint32_t v)
{
	/* Use parallel SWAR algorithm */
	v = v - ((v >> 1) & 0x55555555);
	v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
	return (((v + (v >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24;
}

/**
 * Returns the number of bits set in the provided value.
 * Simple wrapper for DPOP instruction.
 *
 * @param val    64 bit value to count set bits in
 *
 * @return Number of bits set
 */
static inline int bdk_dpop(uint64_t val)
{
	return bdk_pop(val & 0xffffffff) + bdk_pop(val >> 32);
}

/**
 * Wait for the specified number of micro seconds
 *
 * @param usec   micro seconds to wait
 */
static inline void bdk_wait_usec(uint64_t usec)
{
	udelay((unsigned int)usec);
}

#endif	/* !__BDK_BDK_COREBOOT_H */
