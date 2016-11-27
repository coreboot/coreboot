/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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

#ifndef AMD_HT_WRAPPER_H
#define AMD_HT_WRAPPER_H

#include <northbridge/amd/amdfam10/raminit.h>
#include <northbridge/amd/amdfam10/amdfam10.h>
#include <inttypes.h>
#include "h3finit.h"

void amd_ht_fixup(struct sys_info *sysinfo);
u32 get_nodes(void);
void amd_ht_init(struct sys_info *sysinfo);

#endif
