/*
 * Copyright (C) 2013 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <armv7.h>

/*
 * Sets L2 cache related parameters before enabling data cache
 */
void v7_outer_cache_enable(void)
{
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
