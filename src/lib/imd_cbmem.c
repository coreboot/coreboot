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
 */

#include <bootstate.h>
#include <bootmem.h>
#include <compiler.h>
#include <console/console.h>
#include <cbmem.h>
#include <imd.h>
#include <rules.h>
#include <string.h>
#include <stdlib.h>
#include <arch/early_variables.h>

/*
 * We need special handling on x86 where CAR global migration is employed. One
 * cannot use true globals in that circumstance because CAR is where the globals
 * are backed -- creating a circular dependency. For non CAR platforms globals
 * are free to be used as well as any stages that are purely executing out of
 * RAM. For CAR platforms that don't migrate globals the as-linked globals can
 * be used, but they need special decoration using CAR_GLOBAL. That ensures
 * proper object placement in conjunction with the linker.
 *
 * For the CAR global migration platforms we have to always try to partially
 * recover CBMEM from cbmem_top() whenever we try to access it. In other
 * environments we're not so constrained and just keep the backing imd struct
 * in a global. This also means that we can easily tell whether CBMEM has
 * explicitly been initialized or recovered yet on those platforms, and don't
 * need to put the burden on board or chipset code to tell us by returning
 * NULL from cbmem_top() before that point.
 */
#define CAN_USE_GLOBALS \
	(!IS_ENABLED(CONFIG_ARCH_X86) || ENV_RAMSTAGE || ENV_POSTCAR || \
	 IS_ENABLED(CONFIG_NO_CAR_GLOBAL_MIGRATION))

static inline struct imd *cbmem_get_imd(void)
{
	if (CAN_USE_GLOBALS) {
		static struct imd imd_cbmem CAR_GLOBAL;
		return &imd_cbmem;
	}
	return NULL;
}

static inline const struct cbmem_entry *imd_to_cbmem(const struct imd_entry *e)
{
	return (const struct cbmem_entry *)e;
}

static inline const struct imd_entry *cbmem_to_imd(const struct cbmem_entry *e)
{
	return (const struct imd_entry *)e;
}

/* These are the different situations to handle:
 *  CONFIG_EARLY_CBMEM_INIT:
 *      In ramstage cbmem_initialize() attempts a recovery of the
 *      cbmem region set up by romstage. It uses cbmem_top() as the
 *      starting point of recovery.
 *
 *      In romstage, similar to ramstage,  cbmem_initialize() needs to
 *      attempt recovery of the cbmem area using cbmem_top() as the limit.
 *      cbmem_initialize_empty() initializes an empty cbmem area from
 *      cbmem_top();
 *
 */
static struct imd *imd_init_backing(struct imd *backing)
{
	struct imd *imd;

	imd = cbmem_get_imd();

	if (imd != NULL)
		return imd;

	imd = backing;

	return imd;
}

static struct imd *imd_init_backing_with_recover(struct imd *backing)
{
	struct imd *imd;

	imd = imd_init_backing(backing);
	if (!CAN_USE_GLOBALS) {
		/* Always partially recover if we can't keep track of whether
		 * we have already initialized CBMEM in this stage. */
		imd_handle_init(imd, cbmem_top());
		imd_handle_init_partial_recovery(imd);
	}

	return imd;
}

void cbmem_initialize_empty(void)
{
	cbmem_initialize_empty_id_size(0, 0);
}

void __weak cbmem_top_init(void)
{
}

static void cbmem_top_init_once(void)
{
	/* Call one-time hook on expected cbmem init during boot. This sequence
	   assumes first init call is in romstage for early cbmem init and
	   ramstage for late cbmem init. */
	if (IS_ENABLED(CONFIG_EARLY_CBMEM_INIT) && !ENV_ROMSTAGE)
		return;
	if (IS_ENABLED(CONFIG_LATE_CBMEM_INIT) && !ENV_RAMSTAGE)
		return;

	cbmem_top_init();
}

void cbmem_initialize_empty_id_size(u32 id, u64 size)
{
	struct imd *imd;
	struct imd imd_backing;
	const int no_recovery = 0;

	cbmem_top_init_once();

	imd = imd_init_backing(&imd_backing);
	imd_handle_init(imd, cbmem_top());

	printk(BIOS_DEBUG, "CBMEM:\n");

	if (imd_create_tiered_empty(imd, CBMEM_ROOT_MIN_SIZE, CBMEM_LG_ALIGN,
					CBMEM_SM_ROOT_SIZE, CBMEM_SM_ALIGN)) {
		printk(BIOS_DEBUG, "failed.\n");
		return;
	}

	/* Add the specified range first */
	if (size)
		cbmem_add(id, size);

	/* Complete migration to CBMEM. */
	cbmem_run_init_hooks(no_recovery);
}

int cbmem_initialize(void)
{
	return cbmem_initialize_id_size(0, 0);
}

int cbmem_initialize_id_size(u32 id, u64 size)
{
	struct imd *imd;
	struct imd imd_backing;
	const int recovery = 1;

	cbmem_top_init_once();

	imd = imd_init_backing(&imd_backing);
	imd_handle_init(imd, cbmem_top());

	if (imd_recover(imd))
		return 1;

#if defined(__PRE_RAM__)
	/*
	 * Lock the imd in romstage on a recovery. The assumption is that
	 * if the imd area was recovered in romstage then S3 resume path
	 * is being taken.
	 */
	imd_lockdown(imd);
#endif

	/* Add the specified range first */
	if (size)
		cbmem_add(id, size);

	/* Complete migration to CBMEM. */
	cbmem_run_init_hooks(recovery);

	/* Recovery successful. */
	return 0;
}

int cbmem_recovery(int is_wakeup)
{
	int rv = 0;
	if (!is_wakeup)
		cbmem_initialize_empty();
	else
		rv = cbmem_initialize();
	return rv;
}

const struct cbmem_entry *cbmem_entry_add(u32 id, u64 size64)
{
	struct imd *imd;
	struct imd imd_backing;
	const struct imd_entry *e;

	imd = imd_init_backing_with_recover(&imd_backing);

	e = imd_entry_find_or_add(imd, id, size64);

	return imd_to_cbmem(e);
}

void *cbmem_add(u32 id, u64 size)
{
	struct imd *imd;
	struct imd imd_backing;
	const struct imd_entry *e;

	imd = imd_init_backing_with_recover(&imd_backing);

	e = imd_entry_find_or_add(imd, id, size);

	if (e == NULL)
		return NULL;

	return imd_entry_at(imd, e);
}

/* Retrieve a region provided a given id. */
const struct cbmem_entry *cbmem_entry_find(u32 id)
{
	struct imd *imd;
	struct imd imd_backing;
	const struct imd_entry *e;

	imd = imd_init_backing_with_recover(&imd_backing);

	e = imd_entry_find(imd, id);

	return imd_to_cbmem(e);
}

void *cbmem_find(u32 id)
{
	struct imd *imd;
	struct imd imd_backing;
	const struct imd_entry *e;

	imd = imd_init_backing_with_recover(&imd_backing);

	e = imd_entry_find(imd, id);

	if (e == NULL)
		return NULL;

	return imd_entry_at(imd, e);
}

/* Remove a reserved region. Returns 0 on success, < 0 on error. Note: A region
 * cannot be removed unless it was the last one added. */
int cbmem_entry_remove(const struct cbmem_entry *entry)
{
	struct imd *imd;
	struct imd imd_backing;

	imd = imd_init_backing_with_recover(&imd_backing);

	return imd_entry_remove(imd, cbmem_to_imd(entry));
}

u64 cbmem_entry_size(const struct cbmem_entry *entry)
{
	struct imd *imd;
	struct imd imd_backing;

	imd = imd_init_backing_with_recover(&imd_backing);

	return imd_entry_size(imd, cbmem_to_imd(entry));
}

void *cbmem_entry_start(const struct cbmem_entry *entry)
{
	struct imd *imd;
	struct imd imd_backing;

	imd = imd_init_backing_with_recover(&imd_backing);

	return imd_entry_at(imd, cbmem_to_imd(entry));
}

void cbmem_add_bootmem(void)
{
	void *baseptr = NULL;
	size_t size = 0;

	imd_region_used(cbmem_get_imd(), &baseptr, &size);
	bootmem_add_range((uintptr_t)baseptr, size, BM_MEM_TABLE);
}

#if ENV_RAMSTAGE || (IS_ENABLED(CONFIG_EARLY_CBMEM_LIST) \
	&& (ENV_POSTCAR || ENV_ROMSTAGE))
/*
 * -fdata-sections doesn't work so well on read only strings. They all
 * get put in the same section even though those strings may never be
 * referenced in the final binary.
 */
void cbmem_list(void)
{
	static const struct imd_lookup lookup[] = { CBMEM_ID_TO_NAME_TABLE };
	struct imd *imd;
	struct imd imd_backing;

	imd = imd_init_backing_with_recover(&imd_backing);
	imd_print_entries(imd, lookup, ARRAY_SIZE(lookup));
}
#endif

void cbmem_add_records_to_cbtable(struct lb_header *header)
{
	struct imd_cursor cursor;
	struct imd *imd;

	imd = cbmem_get_imd();

	if (imd_cursor_init(imd, &cursor))
		return;

	while (1) {
		const struct imd_entry *e;
		struct lb_cbmem_entry *lbe;
		uint32_t id;

		e = imd_cursor_next(&cursor);

		if (e == NULL)
			break;

		id = imd_entry_id(imd, e);
		/* Don't add these metadata entries. */
		if (id == CBMEM_ID_IMD_ROOT || id == CBMEM_ID_IMD_SMALL)
			continue;

		lbe = (struct lb_cbmem_entry *)lb_new_record(header);
		lbe->tag = LB_TAG_CBMEM_ENTRY;
		lbe->size = sizeof(*lbe);
		lbe->address = (uintptr_t)imd_entry_at(imd, e);
		lbe->entry_size = imd_entry_size(imd, e);
		lbe->id = id;
	}
}
