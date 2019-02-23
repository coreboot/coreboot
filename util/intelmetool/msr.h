/* intelmetool
 *
 * Copyright (C) 2013-2016 Philipp Deppenwiese <zaolin@das-labor.org>
 * Copyright (C) 2013-2016 Alexander Couzens <lynxis@fe80.eu>
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
 */

#include <inttypes.h>

#ifndef __DARWIN__

#define MSR_BOOTGUARD 0x13A

typedef struct {
	unsigned int ebx;
	unsigned int edx;
	unsigned int ecx;
} regs_t;

extern int msr_bootguard(uint64_t *msr, int debug);
#endif
