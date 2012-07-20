/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2000 Ronald G. Minnich
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#ifndef __CPU__INTEL__MICROCODE__
#define __CPU__INTEL__MICROCODE__

#ifndef __PRE_RAM__
#if CONFIG_CPU_MICROCODE_IN_CBFS
void intel_update_microcode_from_cbfs(void);
#else
void intel_update_microcode(const void *microcode_updates);
#endif
#endif

#endif
