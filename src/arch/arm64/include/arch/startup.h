/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc
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
 * Foundation, Inc.
 */

#ifndef __ARCH_ARM64_INCLUDE_ARCH_STARTUP_H__
#define __ARCH_ARM64_INCLUDE_ARCH_STARTUP_H__

/* Every element occupies 8 bytes (64-bit entries) */
#define PER_ELEMENT_SIZE_BYTES		8
#define MAIR_INDEX			0
#define TCR_INDEX			1
#define TTBR0_INDEX			2
#define SCR_INDEX			3
#define VBAR_INDEX			4
#define CNTFRQ_INDEX			5
#define CPTR_INDEX			6
#define CPACR_INDEX			7
/* IMPORTANT!!! If any new element is added please update NUM_ELEMENTS */
#define NUM_ELEMENTS			8

#ifndef __ASSEMBLY__

/*
 * startup_save_cpu_data is used to save register values that need to be setup
 * when a CPU starts booting. This is used by secondary CPUs as well as resume
 * path to directly setup MMU and other related registers.
 */
void startup_save_cpu_data(void);

#endif

#endif /* __ARCH_ARM64_INCLUDE_ARCH_STARTUP_H__ */
