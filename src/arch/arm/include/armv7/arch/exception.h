/* SPDX-License-Identifier: BSD-3-Clause */
/* This file is part of the coreboot project. */

#ifndef _ARCH_EXCEPTION_H
#define _ARCH_EXCEPTION_H

#include <stdint.h>

void exception_init(void);
void set_vbar(uint32_t vbar);

#endif
