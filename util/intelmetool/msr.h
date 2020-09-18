/* intelmetool */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <inttypes.h>

#ifndef __DARWIN__

#define MSR_BOOTGUARD 0x13A

typedef struct {
	unsigned int ebx;
	unsigned int edx;
	unsigned int ecx;
} regs_t;

extern int msr_bootguard(uint64_t *msr);
#endif
