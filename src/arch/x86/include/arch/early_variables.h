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

#ifndef ARCH_EARLY_VARIABLES_H
#define ARCH_EARLY_VARIABLES_H

#ifdef __PRE_RAM__
#define CAR_GLOBAL __attribute__((section(".car.global_data,\"w\",@nobits#")))
#define CAR_CBMEM __attribute__((section(".car.cbmem_console,\"w\",@nobits#")))
#else
#define CAR_GLOBAL
#define CAR_CBMEM
#endif

#if defined(__PRE_RAM__)
#define CAR_MIGRATE_ATTR __attribute__ ((used,section (".car.migrate")))

/* Call migrate_fn_() when CAR globals are migrated. */
#define CAR_MIGRATE(migrate_fn_) \
	static void (* const migrate_fn_ ## _ptr)(void) CAR_MIGRATE_ATTR = \
	migrate_fn_;

/* Get the correct pointer for the CAR global variable. */
void *car_get_var_ptr(void *var);

/* Get and set a primitive type global variable. */
#define car_get_var(var) \
	*(typeof(var) *)car_get_var_ptr(&(var))
#define car_set_var(var, val) \
	do { car_get_var(var) = (val); } while(0)

/* Migrate the CAR variables to memory. */
void car_migrate_variables(void);

#else
#define CAR_MIGRATE(migrate_fn_)
static inline void *car_get_var_ptr(void *var) { return var; }
#define car_get_var(var) (var)
#define car_set_var(var, val) do { (var) = (val); } while (0)
static inline void car_migrate_variables(void) { }
#endif


#endif
