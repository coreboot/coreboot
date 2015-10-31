/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#ifndef __ARCH_MEMLAYOUT_H
#define __ARCH_MEMLAYOUT_H

#include <rules.h>

#if ENV_ROMSTAGE || ENV_VERSTAGE
/* No .data or .bss sections. Cache as ram is handled separately. */
#define ARCH_STAGE_HAS_DATA_SECTION 0
#define ARCH_STAGE_HAS_BSS_SECTION 0
#endif

#endif /* __ARCH_MEMLAYOUT_H */
