/*
 * This file is part of the coreboot project.
 *
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

#include <cbmem.h>

void *cbmem_top(void)
{
	/* Top of cbmem is at lowest usable DRAM address below 4GiB. */
	/* For now, last 1M of 4G */
	void *ptr = (void *) ((1ULL << 32) - 1048576);
	return ptr;
}
