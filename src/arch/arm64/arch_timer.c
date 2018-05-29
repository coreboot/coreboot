/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018, The Linux Foundation.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <timer.h>
#include <arch/lib_helpers.h>

void timer_monotonic_get(struct mono_time *mt)
{
	uint64_t tvalue = raw_read_cntpct_el0();
	uint32_t tfreq  = raw_read_cntfrq_el0();
	long usecs = (tvalue * 1000000) / tfreq;
	mono_time_set_usecs(mt, usecs);
}
