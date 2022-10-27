/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __TYPES_H
#define __TYPES_H

/* types.h is supposed to provide the standard headers defined in here: */
/* IWYU pragma: begin_exports */
#include <commonlib/bsd/cb_err.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
/* IWYU pragma: end_exports */

/*
 * This may mean something else on architectures where the bits are numbered
 * from the MSB (e.g. PowerPC), but until we cross that bridge, this macro is
 * perfectly fine.
 */
#ifndef BIT
#define BIT(x)				(1ul << (x))
#endif

#define BITS_PER_BYTE			8

#endif /* __TYPES_H */
