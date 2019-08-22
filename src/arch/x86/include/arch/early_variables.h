/*
 * This file is part of the coreboot project.
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
#include <commonlib/helpers.h>
#include <stdlib.h>

#if ENV_ROMSTAGE && CONFIG(CAR_GLOBAL_MIGRATION)

/*
 * The _car_global_[start|end]symbols cover CAR data which is relocatable
 * once memory comes online. Variables with CAR_GLOBAL decoration
 * reside within this region.
 */
extern char _car_global_start[];
extern char _car_global_end[];
#define _car_global_size (_car_global_end - _car_global_start)

asm(".section .car.global_data,\"w\",@nobits");
asm(".previous");
#ifdef __clang__
#define CAR_GLOBAL __attribute__((used, section(".car.global_data")))
#else
#define CAR_GLOBAL __attribute__((used, section(".car.global_data#")))
#endif /* __clang__ */

/* Get the correct pointer for the CAR global variable. */
void *car_get_var_ptr(void *var);

/* Return 1 when currently running with globals in Cache-as-RAM, 0 otherwise. */
int car_active(void);

/* Get and set a primitive type global variable. */
#define car_get_var(var) \
	(*(typeof(var) *)car_get_var_ptr(&(var)))
#define car_set_var(var, val)	car_get_var(var) = (val)

/* Get and set a CAR_GLOBAL pointing elsewhere inside CAR. */
#if !CONFIG(PLATFORM_USES_FSP1_0)
#define car_get_ptr car_get_var
#define car_set_ptr car_set_var
#else
void *car_get_reloc_ptr(void *var);
void car_set_reloc_ptr(void *var, void *val);
#define car_get_ptr(var) car_get_reloc_ptr(&(var))
#define car_set_ptr(var, val) car_set_reloc_ptr(&(var), (val))
#endif

static inline size_t car_data_size(void)
{
	size_t car_size = _car_global_size;
	return ALIGN(car_size, 64);
}

static inline size_t car_object_offset(void *ptr)
{
	return (char *)ptr - &_car_global_start[0];
}

#else

/*
 * For all stages other than romstage, all CAR_GLOBAL variables are accessed
 * unconditionally as there is no migration of symbols.
 */

#define CAR_GLOBAL
#define car_get_var(var) (var)
#define car_set_var(var, val)	(var) = (val)
#define car_get_ptr car_get_var
#define car_set_ptr car_set_var

static inline void *car_get_var_ptr(void *var)
{
	return var;
}

static inline int car_active(void)
{
	return ENV_CACHE_AS_RAM;
}

#endif

#endif /* ARCH_EARLY_VARIABLES_H */
