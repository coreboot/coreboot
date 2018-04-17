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
 */

#ifndef ARCH_EARLY_VARIABLES_H
#define ARCH_EARLY_VARIABLES_H

#include <arch/symbols.h>
#include <stdlib.h>
#include <rules.h>

#if ENV_CACHE_AS_RAM
asm(".section .car.global_data,\"w\",@nobits");
asm(".previous");
#ifdef __clang__
#define CAR_GLOBAL __attribute__((used, section(".car.global_data")))
#else
#define CAR_GLOBAL __attribute__((used, section(".car.global_data#")))
#endif /* __clang__ */

/*
 * In stages that use CAR (verstage, C bootblock) all CAR_GLOBAL variables are
 * accessed unconditionally because cbmem is never initialized until romstage
 * when dram comes up.
 */
#if ENV_VERSTAGE || ENV_BOOTBLOCK || IS_ENABLED(CONFIG_NO_CAR_GLOBAL_MIGRATION)
static inline void *car_get_var_ptr(void *var)
{
	return var;
}

static inline void *car_sync_var_ptr(void *var)
{
	return var;
}

static inline int car_active(void)
{
	return 1;
}
#else
/* Get the correct pointer for the CAR global variable. */
void *car_get_var_ptr(void *var);

/* Get and update a CAR_GLOBAL pointing elsewhere in car.global_data*/
void *car_sync_var_ptr(void *var);

/* Return 1 when currently running with globals in Cache-as-RAM, 0 otherwise. */
int car_active(void);
#endif /* ENV_VERSTAGE */

/* Get and set a primitive type global variable. */
#define car_get_var(var) \
	(*(typeof(var) *)car_get_var_ptr(&(var)))
#define car_sync_var(var) \
	(*(typeof(var) *)car_sync_var_ptr(&(var)))
#define car_set_var(var, val)	car_get_var(var) = (val)

static inline size_t car_data_size(void)
{
	size_t car_size = _car_relocatable_data_size;
	return ALIGN(car_size, 64);
}

static inline size_t car_object_offset(void *ptr)
{
	return (char *)ptr - &_car_relocatable_data_start[0];
}

#else
#define CAR_GLOBAL
static inline void *car_get_var_ptr(void *var) { return var; }
static inline int car_active(void) { return 0; }
#define car_get_var(var) (var)
#define car_sync_var(var) (var)
#define car_set_var(var, val)	(var) = (val)
#endif

#endif
