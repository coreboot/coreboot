/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_ARM_LIBGCC_LIBGCC_H__
#define __ARCH_ARM_LIBGCC_LIBGCC_H__

#include <stdint.h>

uint64_t __umoddi3(uint64_t num, uint64_t den);
uint64_t __udivmoddi4(uint64_t num, uint64_t den, uint64_t *rem);

#endif
