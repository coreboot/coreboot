/*
 * This file is part of the coreboot project.
 *
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

#include <arch/cache.h>
#include <soc/cache.h>
#include <stdint.h>

enum {
	L2CTLR_ECC_PARITY = 0x1 << 21,
	L2CTLR_TAG_RAM_LATENCY_MASK = 0x7 << 6,
	L2CTLR_TAG_RAM_LATENCY_CYCLES_3 = 2 << 6,
	L2CTLR_DATA_RAM_LATENCY_MASK = 0x7 << 0,
	L2CTLR_DATA_RAM_LATENCY_CYCLES_3  = 2 << 0
};

enum {
	L2ACTLR_FORCE_L2_LOGIC_CLOCK_ENABLE_ACTIVE = 0x1 << 27,
	L2ACTLR_ENABLE_HAZARD_DETECT_TIMEOUT = 0x1 << 7,
	L2ACTLR_DISABLE_CLEAN_EVICT_PUSH_EXTERNAL = 0x1 << 3
};

/* Configures L2 Control Register to use 3 cycles for DATA/TAG RAM latency. */
static void configure_l2ctlr(void)
{
	uint32_t val;

	val = read_l2ctlr();
	val &= ~(L2CTLR_DATA_RAM_LATENCY_MASK | L2CTLR_TAG_RAM_LATENCY_MASK);
	val |= (L2CTLR_DATA_RAM_LATENCY_CYCLES_3 |
			L2CTLR_TAG_RAM_LATENCY_CYCLES_3 | L2CTLR_ECC_PARITY);
	write_l2ctlr(val);
}

/* Configures L2 Auxiliary Control Register for Cortex A15. */
static void configure_l2actlr(void)
{
	uint32_t val;

	val = read_l2actlr();
	val |= (L2ACTLR_DISABLE_CLEAN_EVICT_PUSH_EXTERNAL |
			L2ACTLR_ENABLE_HAZARD_DETECT_TIMEOUT |
			L2ACTLR_FORCE_L2_LOGIC_CLOCK_ENABLE_ACTIVE);
	write_l2actlr(val);
}

void configure_l2_cache(void)
{
	configure_l2ctlr();
	configure_l2actlr();
}
