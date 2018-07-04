/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2012 The Chromium OS Authors.  All rights reserved.
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

#ifndef SOUTHBRIDGE_INTEL_DEFAULT_RCBA_H
#define SOUTHBRIDGE_INTEL_DEFAULT_RCBA_H

#ifndef __ACPI__
#define DEFAULT_RCBA		((u8 *)0xfed1c000)
#else
#define DEFAULT_RCBA	        0xfed1c000
#endif

#ifndef __ACPI__

#define RCBA8(x) (*((volatile u8 *)(DEFAULT_RCBA + x)))
#define RCBA16(x) (*((volatile u16 *)(DEFAULT_RCBA + x)))
#define RCBA32(x) (*((volatile u32 *)(DEFAULT_RCBA + x)))

#define RCBA_AND_OR(bits, x, and, or) \
	(RCBA##bits(x) = ((RCBA##bits(x) & (and)) | (or)))
#define RCBA8_AND_OR(x, and, or)  RCBA_AND_OR(8, x, and, or)
#define RCBA16_AND_OR(x, and, or) RCBA_AND_OR(16, x, and, or)
#define RCBA32_AND_OR(x, and, or) RCBA_AND_OR(32, x, and, or)
#define RCBA32_OR(x, or) RCBA_AND_OR(32, x, ~0UL, or)


#endif /* __ACPI__ */
#endif /* SOUTHBRIDGE_INTEL_DEFAULT_RCBA_H */
