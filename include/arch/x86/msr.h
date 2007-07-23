/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef CPU_X86_MSR_H
#define CPU_X86_MSR_H

/* standard MSR operations, everyone has written these one hundred times */
struct msr {
	u32 lo;
	u32 hi;
};

struct msrinit {
        u32 msrnum;
        struct msr msr;
};

static inline struct msr rdmsr(u32 index)
{
	struct msr  result;
	__asm__ __volatile__ (
		"rdmsr"
		: "=a" (result.lo), "=d" (result.hi)
		: "c" (index)
		);
	return result;
}

static inline void wrmsr(u32 index, struct msr msr)
{
	__asm__ __volatile__ (
		"wrmsr"
		: /* No outputs */
		: "c" (index), "a" (msr.lo), "d" (msr.hi)
		);
}


#endif /* CPU_X86_MSR_H */
