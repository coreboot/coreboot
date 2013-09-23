/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
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

#include <string.h>
#include <stddef.h>
#include <console/console.h>
#include <cbmem.h>
#include <arch/early_variables.h>

typedef void (* const car_migration_func_t)(void);

extern car_migration_func_t _car_migrate_start;
extern car_migration_func_t _car_migrate_end;

extern char _car_data_start[];
extern char _car_data_end[];

/*
 * The car_migrated global variable determines if the cache-as-ram space has
 * been migrated to real RAM. It does this by assuming the following things:
 *   1. cache-as-ram space is zero'd out once it is set up.
 *   2. Either the cache-as-ram space is memory-backed after getting torn down
 *      or the space returns 0xff's for each byte read.
 * Based on these 2 attributes there is the ability to tell when the
 * cache-as-ram region has been migrated.
 */
static int car_migrated CAR_GLOBAL;


void *car_get_var_ptr(void *var)
{
	char *migrated_base;
	int offset;
	void * _car_start = &_car_data_start;
	void * _car_end = &_car_data_end;

	/* If the cache-as-ram has not been migrated return the pointer
	 * passed in. */
	if (!car_migrated)
		return var;

	if (var < _car_start || var >= _car_end) {
		printk(BIOS_ERR,
		       "Requesting CAR variable outside of CAR region: %p\n",
		       var);
		return var;
	}

	migrated_base = cbmem_find(CBMEM_ID_CAR_GLOBALS);

	if (migrated_base == NULL) {
		printk(BIOS_ERR, "CAR: Could not find migration base!\n");
		return var;
	}

	offset = (char *)var - (char *)_car_start;

	return &migrated_base[offset];
}

void car_migrate_variables(void)
{
	void *migrated_base;
	car_migration_func_t *migrate_func;
	size_t car_data_size = &_car_data_end[0] - &_car_data_start[0];

	/* Check if already migrated. */
	if (car_migrated)
		return;

	migrated_base = cbmem_add(CBMEM_ID_CAR_GLOBALS, car_data_size);

	if (migrated_base == NULL) {
		printk(BIOS_ERR, "Could not migrate CAR data!\n");
		return;
	}

	memcpy(migrated_base, &_car_data_start[0], car_data_size);

	/* Mark that the data has been moved. */
	car_migrated = ~0;

	/* Call all the migration functions. */
	migrate_func = &_car_migrate_start;
	while (migrate_func != &_car_migrate_end) {
		(*migrate_func)();
		migrate_func++;
	}
}
