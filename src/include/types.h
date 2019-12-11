/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __TYPES_H
#define __TYPES_H

/* types.h is supposed to provide the standard headers defined in here: */
#include <commonlib/bsd/cb_err.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/*
 * This may mean something else on architectures where the bits are numbered
 * from the MSB (e.g. PowerPC), but until we cross that bridge, this macro is
 * perfectly fine.
 */
#ifndef BIT
#define BIT(x)				(1ul << (x))
#endif

#endif /* __TYPES_H */
