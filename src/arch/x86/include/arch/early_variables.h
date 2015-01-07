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
 * Foundation, Inc.
 */

#ifndef ARCH_EARLY_VARIABLES_H
#define ARCH_EARLY_VARIABLES_H

#if defined(__PRE_RAM__) && IS_ENABLED(CONFIG_CACHE_AS_RAM)
asm(".section .car.global_data,\"w\",@nobits");
asm(".previous");
#ifdef __clang__
#define CAR_GLOBAL __attribute__((used,section(".car.global_data")))
#else
#define CAR_GLOBAL __attribute__((used,section(".car.global_data#")))
#endif /* __clang__ */

/* Get the correct pointer for the CAR global variable. */
void *car_get_var_ptr(void *var);

/* Get and update a CAR_GLOBAL pointing elsewhere in car.global_data*/
void *car_sync_var_ptr(void *var);

/* Get and set a primitive type global variable. */
#define car_get_var(var) \
	*(typeof(var) *)car_get_var_ptr(&(var))
#define car_sync_var(var) \
	*(typeof (var) *)car_sync_var_ptr(&(var))
#define car_set_var(var, val) \
	do { car_get_var(var) = (val); } while(0)

#else
#define CAR_GLOBAL
static inline void *car_get_var_ptr(void *var) { return var; }
#define car_get_var(var) (var)
#define car_sync_var(var) (var)
#define car_set_var(var, val) do { (var) = (val); } while (0)
#endif

#endif
