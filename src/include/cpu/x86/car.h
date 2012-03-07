/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#ifndef CPU_X86_CAR_H
#define CPU_X86_CAR_H

#ifdef __PRE_RAM__
#define CAR_GLOBAL __attribute__((section(".car.global_data,\"w\",@nobits#")))
#define CAR_CBMEM __attribute__((section(".car.cbmem_console,\"w\",@nobits#")))
#else
#define CAR_GLOBAL
#define CAR_CBMEM
#endif

#endif
