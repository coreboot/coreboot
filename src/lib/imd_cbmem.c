/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <boot/coreboot_tables.h>
#include <bootmem.h>
#include <console/console.h>
#include <cbmem.h>
#include <imd.h>
#include <lib.h>
#include <types.h>

/* The program loader passes on cbmem_top and the program entry point
   has to fill in the _cbmem_top_ptr symbol based on the calling arguments. */
uintptr_t _cbmem_top_ptr;

static struct imd imd;

void *cbmem_top(void)
{
	if (ENV_CREATES_CBMEM) {
		static uintptr_t top;
		if (top)
			return (void *)top;
		top = cbmem_top_chipset();
		return (void *)top;
	}
	if (ENV_POSTCAR || ENV_RAMSTAGE)
		return (void *)_cbmem_top_ptr;

	dead_code();
}

int cbmem_initialized;

static inline const struct cbmem_entry *imd_to_cbmem(const struct imd_entry *e)
{
	return (const struct cbmem_entry *)e;
}

static inline const struct imd_entry *cbmem_to_imd(const struct cbmem_entry *e)
{
	return (const struct imd_entry *)e;
}

void cbmem_initialize_empty(void)
{
	cbmem_initialize_empty_id_size(0, 0);
}

static void cbmem_top_init_once(void)
{
	/* Call one-time hook on expected cbmem init during boot. */
	if (!ENV_CREATES_CBMEM)
		return;

	/* The test is only effective on X86 and when address hits UC memory. */
	if (ENV_X86)
		quick_ram_check_or_die((uintptr_t)cbmem_top() - sizeof(u32));
}

void cbmem_initialize_empty_id_size(u32 id, u64 size)
{
	const int no_recovery = 0;

	cbmem_top_init_once();

	imd_handle_init(&imd, cbmem_top());

	printk(BIOS_DEBUG, "CBMEM:\n");

	if (imd_create_tiered_empty(&imd, CBMEM_ROOT_MIN_SIZE, CBMEM_LG_ALIGN,
					CBMEM_SM_ROOT_SIZE, CBMEM_SM_ALIGN)) {
		printk(BIOS_DEBUG, "failed.\n");
		return;
	}

	/* Add the specified range first */
	if (size)
		cbmem_add(id, size);

	/* Complete migration to CBMEM. */
	cbmem_run_init_hooks(no_recovery);

	cbmem_initialized = 1;
}

int cbmem_initialize(void)
{
	return cbmem_initialize_id_size(0, 0);
}

int cbmem_initialize_id_size(u32 id, u64 size)
{
	const int recovery = 1;

	cbmem_top_init_once();

	imd_handle_init(&imd, cbmem_top());

	if (imd_recover(&imd))
		return 1;

	/*
	 * Lock the imd in romstage on a recovery. The assumption is that
	 * if the imd area was recovered in romstage then S3 resume path
	 * is being taken.
	 */
	if (ENV_CREATES_CBMEM)
		imd_lockdown(&imd);

	/* Add the specified range first */
	if (size)
		cbmem_add(id, size);

	/* Complete migration to CBMEM. */
	cbmem_run_init_hooks(recovery);

	cbmem_initialized = 1;

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
	const struct imd_entry *e;

	e = imd_entry_find_or_add(&imd, id, size64);

	return imd_to_cbmem(e);
}

void *cbmem_add(u32 id, u64 size)
{
	const struct imd_entry *e;

	e = imd_entry_find_or_add(&imd, id, size);

	if (e == NULL)
		return NULL;

	return imd_entry_at(&imd, e);
}

/* Retrieve a region provided a given id. */
const struct cbmem_entry *cbmem_entry_find(u32 id)
{
	const struct imd_entry *e;

	e = imd_entry_find(&imd, id);

	return imd_to_cbmem(e);
}

void *cbmem_find(u32 id)
{
	const struct imd_entry *e;

	e = imd_entry_find(&imd, id);

	if (e == NULL)
		return NULL;

	return imd_entry_at(&imd, e);
}

/* Remove a reserved region. Returns 0 on success, < 0 on error. Note: A region
 * cannot be removed unless it was the last one added. */
int cbmem_entry_remove(const struct cbmem_entry *entry)
{
	return imd_entry_remove(&imd, cbmem_to_imd(entry));
}

u64 cbmem_entry_size(const struct cbmem_entry *entry)
{
	return imd_entry_size(cbmem_to_imd(entry));
}

void *cbmem_entry_start(const struct cbmem_entry *entry)
{
	return imd_entry_at(&imd, cbmem_to_imd(entry));
}

void cbmem_add_bootmem(void)
{
	void *baseptr = NULL;
	size_t size = 0;

	cbmem_get_region(&baseptr, &size);
	bootmem_add_range((uintptr_t)baseptr, size, BM_MEM_TABLE);
}

void cbmem_get_region(void **baseptr, size_t *size)
{
	imd_region_used(&imd, baseptr, size);
}

#if ENV_PAYLOAD_LOADER || (CONFIG(EARLY_CBMEM_LIST) && ENV_HAS_CBMEM)
/*
 * -fdata-sections doesn't work so well on read only strings. They all
 * get put in the same section even though those strings may never be
 * referenced in the final binary.
 */
void cbmem_list(void)
{
	static const struct imd_lookup lookup[] = { CBMEM_ID_TO_NAME_TABLE };

	imd_print_entries(&imd, lookup, ARRAY_SIZE(lookup));
}
#endif

void cbmem_add_records_to_cbtable(struct lb_header *header)
{
	struct imd_cursor cursor;

	if (imd_cursor_init(&imd, &cursor))
		return;

	while (1) {
		const struct imd_entry *e;
		struct lb_cbmem_entry *lbe;
		uint32_t id;

		e = imd_cursor_next(&cursor);

		if (e == NULL)
			break;

		id = imd_entry_id(e);
		/* Don't add these metadata entries. */
		if (id == CBMEM_ID_IMD_ROOT || id == CBMEM_ID_IMD_SMALL)
			continue;

		lbe = (struct lb_cbmem_entry *)lb_new_record(header);
		lbe->tag = LB_TAG_CBMEM_ENTRY;
		lbe->size = sizeof(*lbe);
		lbe->address = (uintptr_t)imd_entry_at(&imd, e);
		lbe->entry_size = imd_entry_size(e);
		lbe->id = id;
	}
}
