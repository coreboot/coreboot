/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 SUSE LINUX AG
 * Copyright (C) 2004 Nick Barker
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>,
 * Raptor Engineering
 * Copyright (C) 2016 Siemens AG
 * (Written by Stefan Reinauer <stepan@coresystems.de>)
 * Copyright 2018-present Facebook, Inc.
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

#ifndef __ARCH_ACPI_H_
#define __ARCH_ACPI_H_

/* STUB */

static inline int acpi_is_wakeup(void) { return 0; }
static inline int acpi_is_wakeup_s3(void) { return 0; }
static inline int acpi_is_wakeup_s4(void) { return 0; }

#endif  /* __ARCH_ACPI_H_ */
