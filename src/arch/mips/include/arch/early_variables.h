/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
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

#ifndef __MIPS_ARCH_EARLY_VARIABLES_H
#define __MIPS_ARCH_EARLY_VARIABLES_H

#define CAR_GLOBAL
#define CAR_MIGRATE(migrate_fn_)

static inline void *car_get_var_ptr(void *var) { return var; }
#define car_get_var(var) (var)
#define car_sync_var(var) (var)
#define car_set_var(var, val) { (var) = (val); }

#endif /* __MIPS_ARCH_EARLY_VARIABLES_H */
