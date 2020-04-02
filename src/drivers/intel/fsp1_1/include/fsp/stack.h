/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _COMMON_STACK_H_
#define _COMMON_STACK_H_

#include <stdint.h>

static inline void *stack_push32(void *stack, uint32_t value)
{
	uint32_t *stack32 = stack;

	stack32 = &stack32[-1];
	*stack32 = value;
	return stack32;
}

#endif /* _COMMON_STACK_H_ */
