/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Kyösti Mälkki <kyosti.malkki@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <cpu/x86/msr.h>

/* Storage for MSRs that need to be replicated over
 * all CPUs after power-on and S2/S3 resumes.
 */
static struct msr_non_volatile_struct {
	uint64_t	tolm;		/* Top of low RAM < 4GB. */
	uint64_t	tom;		/* Top of RAM. */

#if 0
	msrinit_t	mtrr[MAX_MTRRS];	/* TODO */
#endif
} msr_non_volatile;

void msr_nv_setup_ramtop(uint64_t tolm, uint64_t tom)
{
	msr_non_volatile.tolm = tolm;
	msr_non_volatile.tom = tom;
}

uint64_t msr_nv_get_tolm(void)
{
	return msr_non_volatile.tolm;
}

uint64_t msr_nv_get_tom(void)
{
	return msr_non_volatile.tom;
}

