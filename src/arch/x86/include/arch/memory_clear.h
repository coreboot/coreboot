/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef MEMORY_CLEAR_H
#define MEMORY_CLEAR_H

#include <memrange.h>

int arch_clear_memranges(const struct memranges *mem_reserved);

#endif /* MEMORY_CLEAR_H */
