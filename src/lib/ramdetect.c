/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <types.h>
#include <symbols.h>
#include <device/mmio.h>
#include <ramdetect.h>
#include <console/console.h>

#define OVERLAP(a, b, s, e) ((b) > (s) && (a) < (e))

int __weak probe_mb(const uintptr_t dram_start, const uintptr_t size)
{
	uintptr_t addr = dram_start + (size * MiB) - sizeof(uint32_t);
	static const uint32_t patterns[] = {
		0x55aa55aa,
		0x12345678
	};
	void *ptr = (void *) addr;
	size_t i;

	/* Don't accidentally clober oneself. */
	if (OVERLAP(addr, addr + sizeof(uint32_t), (uintptr_t)_program, (uintptr_t) _eprogram))
		return 1;

	uint32_t old = read32(ptr);
	for (i = 0; i < ARRAY_SIZE(patterns); i++) {
		write32(ptr, patterns[i]);
		if (read32(ptr) != patterns[i])
			break;
	}

	write32(ptr, old);
	return i == ARRAY_SIZE(patterns);
}

/* - 20 as probe_size is in MiB, - 1 as i is signed */
#define MAX_ADDRESSABLE_SPACE (sizeof(size_t) * 8 - 20 - 1)

/* Probe an area if it's read/writable. */
size_t probe_ramsize(const uintptr_t dram_start, const size_t probe_size)
{
	ssize_t i;
	size_t msb = 0;
	size_t discovered = 0;

	static size_t saved_result;
	if (saved_result)
		return saved_result;

	/* Find the MSB + 1. */
	size_t tmp = probe_size;
	do {
		msb++;
	} while (tmp >>= 1);

	/* Limit search to accessible address space */
	msb = MIN(msb, MAX_ADDRESSABLE_SPACE);

	/* Compact binary search.  */
	for (i = msb; i >= 0; i--)
		if (probe_mb(dram_start, (discovered | (1ULL << i))))
			discovered |= (1ULL << i);

	saved_result = discovered;
	printk(BIOS_DEBUG, "RAMDETECT: Found %zu MiB RAM\n", discovered);
	return discovered;
}
