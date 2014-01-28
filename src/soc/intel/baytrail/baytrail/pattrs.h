/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _PATTRS_H_
#define _PATTRS_H_

#include <stdint.h>
#include <cpu/x86/msr.h>

enum {
	IACORE_MIN,
	IACORE_LFM,
	IACORE_MAX,
	IACORE_TURBO,
	IACORE_END
};

/* The pattrs structure is a common place to stash pertinent information
 * about the processor or platform. Instead of going to the source (msrs, cpuid)
 * every time an attribute is needed use the pattrs structure.
 */
struct pattrs {
	msr_t platform_id;
	msr_t platform_info;
	int iacore_ratios[IACORE_END];
	int iacore_vids[IACORE_END];
	uint32_t cpuid;
	int revid;
	int stepping;
	const void *microcode_patch;
	int address_bits;
	int num_cpus;
	unsigned bclk_khz;
};

/* This is just to hide the abstraction w/o relying on how the underlying
 * storage is allocated. */
#define PATTRS_GLOB_NAME __global_pattrs
#define DEFINE_PATTRS struct pattrs PATTRS_GLOB_NAME
extern DEFINE_PATTRS;

static inline const struct pattrs *pattrs_get(void)
{
	return &PATTRS_GLOB_NAME;
}


#endif /* _PATTRS_H_ */

