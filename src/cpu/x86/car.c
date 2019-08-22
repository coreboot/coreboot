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

#include <string.h>
#include <stddef.h>
#include <console/console.h>
#include <cbmem.h>
#include <arch/early_variables.h>
#include <symbols.h>

#if CONFIG(PLATFORM_USES_FSP1_0)
#include <drivers/intel/fsp1_0/fsp_util.h>
#endif
typedef void (* const car_migration_func_t)(void);

extern car_migration_func_t _car_migrate_start;

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

/** @brief returns pointer to a CAR variable, before or after migration.
 *
 * @param var pointer to the CAR variable
 */
void *car_get_var_ptr(void *var)
{
	char *migrated_base = NULL;
	int offset;
	void *_car_start = _car_global_start;
	void *_car_end = _car_global_end;

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

#if CONFIG(PLATFORM_USES_FSP1_0)
	migrated_base = (char *)find_saved_temp_mem(
			*(void **)CBMEM_FSP_HOB_PTR);
	/* FSP 1.0 migrates the entire DCACHE RAM */
	offset = (char *)var - (char *)CONFIG_DCACHE_RAM_BASE;
#else
	migrated_base = cbmem_find(CBMEM_ID_CAR_GLOBALS);
	offset = (char *)var - (char *)_car_start;
#endif

	if (migrated_base == NULL)
		die("CAR: Could not find migration base!\n");

	return &migrated_base[offset];
}

#if CONFIG(PLATFORM_USES_FSP1_0)
/*
 * When a CAR_GLOBAL points to target object inside CAR, use relative
 * addressing. Such CAR_GLOBAL has to be expicitly accessed using
 * car_set_reloc_ptr() and car_get_reloc_ptr() as the stored value is now
 * an offset instead of the absolute address (pointer) of the target.
 *
 * This way discovery of objects that are not CAR_GLOBALs themselves,
 * remain discoverable after CAR migration has implicitly happened.
 */
void car_set_reloc_ptr(void *var, void *val)
{
	uintptr_t *offset = car_get_var_ptr(var);
	*offset = 0;

	if (val)
		*offset = (uintptr_t)offset - (uintptr_t)val;
}

void *car_get_reloc_ptr(void *var)
{
	uintptr_t *offset = car_get_var_ptr(var);
	void *val = NULL;

	if (*offset)
		val = (void *)((uintptr_t)offset - *offset);

	return val;
}
#endif

int car_active(void)
{
	return !car_migrated;
}

static void do_car_migrate_variables(void)
{
	void *migrated_base;
	size_t car_size = car_data_size();

	/* Check if already migrated. */
	if (car_migrated)
		return;

	migrated_base = cbmem_add(CBMEM_ID_CAR_GLOBALS, car_size);

	if (migrated_base == NULL) {
		printk(BIOS_ERR, "Could not migrate CAR data!\n");
		return;
	}

	memcpy(migrated_base, _car_global_start, car_size);

	/* Mark that the data has been moved. */
	car_migrated = ~0;
}

static void car_migrate_variables(int is_recovery)
{
	if (!CONFIG(PLATFORM_USES_FSP1_0))
		do_car_migrate_variables();
}
ROMSTAGE_CBMEM_INIT_HOOK(car_migrate_variables)
