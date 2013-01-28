/*
 * Copyright (c) 2012 Samsung Electronics.
 * Arun Mankuzhi <arun.m@samsung.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <system.h>

#include <armv7.h>

enum l2_cache_params {
	CACHE_TAG_RAM_SETUP = (1<<9),
	CACHE_DATA_RAM_SETUP = (1<<5),
	CACHE_TAG_RAM_LATENCY = (2<<6),
	CACHE_DATA_RAM_LATENCY = (2<<0)
};


/* FIXME(dhendrix): maybe move this to a romstage-specific file? */
#ifdef __PRE_RAM__
void enable_caches(void)
{
	/* Enable D-cache. I-cache is already enabled in start.S */
	/* can't use it anyway -- it has dependencies we have to fix. */
	//dcache_enable();
}
#endif

/*
 * Set L2 cache parameters
 */
static void exynos5_set_l2cache_params(void)
{
	unsigned int val = 0;

	asm volatile("mrc p15, 1, %0, c9, c0, 2\n" : "=r"(val));

	val |= CACHE_TAG_RAM_SETUP |
		CACHE_DATA_RAM_SETUP |
		CACHE_TAG_RAM_LATENCY |
		CACHE_DATA_RAM_LATENCY;

	asm volatile("mcr p15, 1, %0, c9, c0, 2\n" : : "r"(val));
}

/*
 * Sets L2 cache related parameters before enabling data cache
 */
void v7_outer_cache_enable(void)
{
	exynos5_set_l2cache_params();
}

/* stubs so we don't need weak symbols in cache_v7.c */
void v7_outer_cache_disable(void)
{
}

void v7_outer_cache_flush_all(void)
{
}

void v7_outer_cache_inval_all(void)
{
}

void v7_outer_cache_flush_range(u32 start, u32 end)
{
}

void v7_outer_cache_inval_range(u32 start, u32 end)
{
}
