/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>

/* This will get updated by ifittool later on to point to the cbfs 'intel_fit' file. */
__attribute__((used, __section__(".fit_pointer"))) const uint64_t fit_ptr = 0;
