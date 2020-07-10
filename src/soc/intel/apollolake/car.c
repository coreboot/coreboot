/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <intelblocks/msr.h>
#include <program_loading.h>
#include <soc/cpu.h>

/*
 * This file supports the necessary hoops one needs to jump through since
 * early FSP component and early stages are running from cache-as-ram.
 */

static inline int is_car_addr(uintptr_t addr)
{
	return ((addr >= CONFIG_DCACHE_RAM_BASE) &&
		(addr < (CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE)));
}

void platform_segment_loaded(uintptr_t start, size_t size, int flags)
{
	/* Bail out if this is not the final segment. */
	if (!(flags & SEG_FINAL))
		return;

	char start_car_check = is_car_addr(start);
	char end_car_check = is_car_addr(start + size - 1);

	/* Bail out if loaded program segment does not lie in CAR region. */
	if (!start_car_check && !end_car_check)
		return;

	/* Loaded program segment should lie entirely within CAR region. */
	assert(start_car_check && end_car_check);

	flush_l1d_to_l2();
}
