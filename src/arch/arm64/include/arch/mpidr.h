/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_MPIDR_H__
#define __ARCH_MPIDR_H__

#include <stdint.h>
#include <arch/lib_helpers.h>

enum {
	MPIDR_RES1_SHIFT = 31,
	MPIDR_U_SHIFT = 30,
	MPIDR_MT_SHIFT = 24,

	MPIDR_AFFINITY_0_SHIFT = 0,
	MPIDR_AFFINITY_1_SHIFT = 8,
	MPIDR_AFFINITY_2_SHIFT = 16,
	MPIDR_AFFINITY_3_SHIFT = 32,
	MPIDR_AFFINITY_MASK = 0xff,
};

static inline uint64_t mpidr_mask(uint8_t aff3, uint8_t aff2,
				  uint8_t aff1, uint8_t aff0)
{
	uint64_t mpidr = 0;

	mpidr |= (uint64_t)aff3 << MPIDR_AFFINITY_3_SHIFT;
	mpidr |= (uint64_t)aff2 << MPIDR_AFFINITY_2_SHIFT;
	mpidr |= (uint64_t)aff1 << MPIDR_AFFINITY_1_SHIFT;
	mpidr |= (uint64_t)aff0 << MPIDR_AFFINITY_0_SHIFT;

	return mpidr;
}

static inline uint64_t read_mpidr(void)
{
	return raw_read_mpidr_el1();
}

static inline uint64_t read_affinity_mpidr(void)
{
	uint64_t affinity_mask;
	affinity_mask = mpidr_mask(MPIDR_AFFINITY_MASK, MPIDR_AFFINITY_MASK,
				   MPIDR_AFFINITY_MASK, MPIDR_AFFINITY_MASK);
	return read_mpidr() & affinity_mask;
}

#endif /* __ARCH_MPIDR_H__ */
