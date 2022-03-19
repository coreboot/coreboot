/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdlib.h>
#include <types.h>
#include <string.h>
#include <tests/test.h>
#include <imd.h>
#include <imd_private.h>
#include <cbmem.h>
#include <commonlib/bsd/helpers.h>
#include <lib.h>

/* Auxiliary functions and definitions. */

#define LG_ROOT_SIZE                                                                           \
	align_up_pow2(sizeof(struct imd_root_pointer) + sizeof(struct imd_root)                \
		      + 3 * sizeof(struct imd_entry))
#define LG_ENTRY_ALIGN (2 * sizeof(int32_t))
#define LG_ENTRY_SIZE (2 * sizeof(int32_t))
#define LG_ENTRY_ID 0xA001

#define SM_ROOT_SIZE LG_ROOT_SIZE
#define SM_ENTRY_ALIGN sizeof(uint32_t)
#define SM_ENTRY_SIZE sizeof(uint32_t)
#define SM_ENTRY_ID 0xB001

#define INVALID_REGION_ID 0xC001

static uint32_t align_up_pow2(uint32_t x)
{
	return (1 << log2_ceil(x));
}

static size_t max_entries(size_t root_size)
{
	return (root_size - sizeof(struct imd_root_pointer) - sizeof(struct imd_root))
	       / sizeof(struct imd_entry);
}

/*
 * Mainly, we should check that imd_handle_init() aligns upper_limit properly
 * for various inputs. Upper limit is the _exclusive_ address, so we expect
 * ALIGN_DOWN.
 */
static void test_imd_handle_init(void **state)
{
	int i;
	void *base;
	struct imd imd;
	uintptr_t test_inputs[] = {
		0,		     /* Lowest possible address */
		0xA000,		     /* Fits in 16 bits, should not get rounded down */
		0xDEAA,		     /* Fits in 16 bits */
		0xB0B0B000,	     /* Fits in 32 bits, should not get rounded down */
		0xF0F0F0F0,	     /* Fits in 32 bits */
		((1ULL << 32) + 4),  /* Just above 32-bit limit */
		0x6666777788889000,  /* Fits in 64 bits, should not get rounded down */
		((1ULL << 60) - 100) /* Very large address, fitting in 64 bits */
	};

	for (i = 0; i < ARRAY_SIZE(test_inputs); i++) {
		base = (void *)test_inputs[i];

		imd_handle_init(&imd, (void *)base);

		assert_int_equal(imd.lg.limit % LIMIT_ALIGN, 0);
		assert_int_equal(imd.lg.limit, ALIGN_DOWN(test_inputs[i], LIMIT_ALIGN));
		assert_ptr_equal(imd.lg.r, NULL);

		/* Small allocations not initialized */
		assert_ptr_equal(imd.sm.limit, NULL);
		assert_ptr_equal(imd.sm.r, NULL);
	}
}

static void test_imd_handle_init_partial_recovery(void **state)
{
	void *base;
	struct imd imd = {0};
	const struct imd_entry *entry;

	imd_handle_init_partial_recovery(&imd);
	assert_null(imd.lg.limit);
	assert_null(imd.sm.limit);

	base = malloc(LIMIT_ALIGN);
	if (base == NULL)
		fail_msg("Cannot allocate enough memory - fail test");

	imd_handle_init(&imd, (void *)(LIMIT_ALIGN + (uintptr_t)base));
	imd_handle_init_partial_recovery(&imd);

	assert_non_null(imd.lg.r);
	assert_null(imd.sm.limit);

	assert_int_equal(0, imd_create_empty(&imd, LG_ROOT_SIZE, LG_ENTRY_ALIGN));
	entry = imd_entry_add(&imd, SMALL_REGION_ID, LG_ENTRY_SIZE);
	assert_non_null(entry);

	imd_handle_init_partial_recovery(&imd);

	assert_non_null(imd.lg.r);
	assert_non_null(imd.sm.limit);
	assert_ptr_equal(imd.lg.r + entry->start_offset + LG_ENTRY_SIZE, imd.sm.limit);
	assert_non_null(imd.sm.r);

	free(base);
}

static void test_imd_create_empty(void **state)
{
	struct imd imd = {0};
	void *base;
	struct imd_root *r;
	struct imd_entry *e;

	/* Expect imd_create_empty to fail, since imd handle is not initialized */
	assert_int_equal(-1, imd_create_empty(&imd, LG_ROOT_SIZE, LG_ENTRY_ALIGN));
	base = malloc(sizeof(struct imd_root_pointer) + sizeof(struct imd_root));
	if (base == NULL)
		fail_msg("Cannot allocate enough memory - fail test");

	imd_handle_init(&imd, (void *)(LIMIT_ALIGN + (uintptr_t)base));

	/* Try incorrect sizes */
	assert_int_equal(
		-1, imd_create_empty(&imd, sizeof(struct imd_root_pointer), LG_ENTRY_ALIGN));
	assert_int_equal(-1, imd_create_empty(&imd, LG_ROOT_SIZE, 2 * LG_ROOT_SIZE));

	/* Working case */
	assert_int_equal(0, imd_create_empty(&imd, LG_ROOT_SIZE, LG_ENTRY_ALIGN));

	/* Only large allocation initialized with one entry for the root region */
	r = (struct imd_root *)(imd.lg.r);
	assert_non_null(r);

	e = &r->entries[r->num_entries - 1];

	assert_int_equal(max_entries(LG_ROOT_SIZE), r->max_entries);
	assert_int_equal(1, r->num_entries);
	assert_int_equal(0, r->flags);
	assert_int_equal(LG_ENTRY_ALIGN, r->entry_align);
	assert_int_equal(0, r->max_offset);
	assert_ptr_equal(e, &r->entries);

	assert_int_equal(IMD_ENTRY_MAGIC, e->magic);
	assert_int_equal(0, e->start_offset);
	assert_int_equal(LG_ROOT_SIZE, e->size);
	assert_int_equal(CBMEM_ID_IMD_ROOT, e->id);

	free(base);
}

static void test_imd_create_tiered_empty(void **state)
{
	void *base;
	size_t sm_region_size, lg_region_wrong_size;
	struct imd imd = {0};
	struct imd_root *r;
	struct imd_entry *fst_lg_entry, *snd_lg_entry, *sm_entry;

	/* Uninitialized imd handle */
	assert_int_equal(-1, imd_create_tiered_empty(&imd, LG_ROOT_SIZE, LG_ENTRY_ALIGN,
						     LG_ROOT_SIZE, SM_ENTRY_ALIGN));

	base = malloc(LIMIT_ALIGN);
	if (base == NULL)
		fail_msg("Cannot allocate enough memory - fail test");

	imd_handle_init(&imd, (void *)(LIMIT_ALIGN + (uintptr_t)base));

	/* Too small root_size for small region */
	assert_int_equal(-1, imd_create_tiered_empty(&imd, LG_ROOT_SIZE, LG_ENTRY_ALIGN,
						     sizeof(int32_t), 2 * sizeof(int32_t)));

	/* Fail when large region doesn't have capacity for more than 1 entry */
	lg_region_wrong_size = sizeof(struct imd_root_pointer) + sizeof(struct imd_root)
			       + sizeof(struct imd_entry);
	expect_assert_failure(imd_create_tiered_empty(
		&imd, lg_region_wrong_size, LG_ENTRY_ALIGN, SM_ROOT_SIZE, SM_ENTRY_ALIGN));

	assert_int_equal(0, imd_create_tiered_empty(&imd, LG_ROOT_SIZE, LG_ENTRY_ALIGN,
						    SM_ROOT_SIZE, SM_ENTRY_ALIGN));

	r = imd.lg.r;

	/* One entry for root_region and one for small allocations */
	assert_int_equal(2, r->num_entries);

	fst_lg_entry = &r->entries[0];
	assert_int_equal(IMD_ENTRY_MAGIC, fst_lg_entry->magic);
	assert_int_equal(0, fst_lg_entry->start_offset);
	assert_int_equal(LG_ROOT_SIZE, fst_lg_entry->size);
	assert_int_equal(CBMEM_ID_IMD_ROOT, fst_lg_entry->id);

	/* Calculated like in imd_create_tiered_empty */
	sm_region_size = max_entries(SM_ROOT_SIZE) * SM_ENTRY_ALIGN;
	sm_region_size += SM_ROOT_SIZE;
	sm_region_size = ALIGN_UP(sm_region_size, LG_ENTRY_ALIGN);

	snd_lg_entry = &r->entries[1];
	assert_int_equal(IMD_ENTRY_MAGIC, snd_lg_entry->magic);
	assert_int_equal(-sm_region_size, snd_lg_entry->start_offset);
	assert_int_equal(CBMEM_ID_IMD_SMALL, snd_lg_entry->id);

	assert_int_equal(sm_region_size, snd_lg_entry->size);

	r = imd.sm.r;
	assert_int_equal(1, r->num_entries);

	sm_entry = &r->entries[0];
	assert_int_equal(IMD_ENTRY_MAGIC, sm_entry->magic);
	assert_int_equal(0, sm_entry->start_offset);
	assert_int_equal(SM_ROOT_SIZE, sm_entry->size);
	assert_int_equal(CBMEM_ID_IMD_ROOT, sm_entry->id);

	free(base);
}

/* Tests for imdr_recover. */
static void test_imd_recover(void **state)
{
	int32_t offset_copy, max_offset_copy;
	uint32_t rp_magic_copy, num_entries_copy;
	uint32_t e_align_copy, e_magic_copy, e_id_copy;
	uint32_t size_copy, diff;
	void *base;
	struct imd imd = {0};
	struct imd_root_pointer *rp;
	struct imd_root *r;
	struct imd_entry *lg_root_entry, *sm_root_entry, *ptr;
	const struct imd_entry *lg_entry;

	/* Fail when the limit for lg was not set. */
	imd.lg.limit = (uintptr_t)NULL;
	assert_int_equal(-1, imd_recover(&imd));

	/* Set the limit for lg. */
	base = malloc(LIMIT_ALIGN);
	if (base == NULL)
		fail_msg("Cannot allocate enough memory - fail test");

	imd_handle_init(&imd, (void *)(LIMIT_ALIGN + (uintptr_t)base));

	/* Fail when the root pointer is not valid. */
	rp = (void *)imd.lg.limit - sizeof(struct imd_root_pointer);
	assert_non_null(rp);
	assert_int_equal(IMD_ROOT_PTR_MAGIC, rp->magic);

	rp_magic_copy = rp->magic;
	rp->magic = 0;
	assert_int_equal(-1, imd_recover(&imd));
	rp->magic = rp_magic_copy;

	/* Set the root pointer. */
	assert_int_equal(0, imd_create_tiered_empty(&imd, LG_ROOT_SIZE, LG_ENTRY_ALIGN,
						    SM_ROOT_SIZE, SM_ENTRY_ALIGN));
	assert_int_equal(2, ((struct imd_root *)imd.lg.r)->num_entries);
	assert_int_equal(1, ((struct imd_root *)imd.sm.r)->num_entries);

	/* Fail if the number of entries exceeds the maximum number of entries. */
	r = imd.lg.r;
	num_entries_copy = r->num_entries;
	r->num_entries = r->max_entries + 1;
	assert_int_equal(-1, imd_recover(&imd));
	r->num_entries = num_entries_copy;

	/* Fail if entry align is not a power of 2.  */
	e_align_copy = r->entry_align;
	r->entry_align++;
	assert_int_equal(-1, imd_recover(&imd));
	r->entry_align = e_align_copy;

	/* Fail when an entry is not valid. */
	lg_root_entry = &r->entries[0];
	e_magic_copy = lg_root_entry->magic;
	lg_root_entry->magic = 0;
	assert_int_equal(-1, imd_recover(&imd));
	lg_root_entry->magic = e_magic_copy;

	/* Add new entries: large and small. */
	lg_entry = imd_entry_add(&imd, LG_ENTRY_ID, LG_ENTRY_SIZE);
	assert_non_null(lg_entry);
	assert_int_equal(3, r->num_entries);

	assert_non_null(imd_entry_add(&imd, SM_ENTRY_ID, SM_ENTRY_SIZE));
	assert_int_equal(2, ((struct imd_root *)imd.sm.r)->num_entries);

	/* Fail when start_addr is lower than low_limit. */
	r = imd.lg.r;
	max_offset_copy = r->max_offset;
	r->max_offset = lg_entry->start_offset + sizeof(int32_t);
	assert_int_equal(-1, imd_recover(&imd));
	r->max_offset = max_offset_copy;

	/* Fail when start_addr is at least imdr->limit. */
	offset_copy = lg_entry->start_offset;
	ptr = (struct imd_entry *)lg_entry;
	ptr->start_offset = (void *)imd.lg.limit - (void *)r;
	assert_int_equal(-1, imd_recover(&imd));
	ptr->start_offset = offset_copy;

	/* Fail when (start_addr + e->size) is higher than imdr->limit. */
	size_copy = lg_entry->size;
	diff = (void *)imd.lg.limit - ((void *)r + lg_entry->start_offset);
	ptr->size = diff + 1;
	assert_int_equal(-1, imd_recover(&imd));
	ptr->size = size_copy;

	/* Succeed if small region is not present. */
	sm_root_entry = &r->entries[1];
	e_id_copy = sm_root_entry->id;
	sm_root_entry->id = 0;
	assert_int_equal(0, imd_recover(&imd));
	sm_root_entry->id = e_id_copy;

	assert_int_equal(0, imd_recover(&imd));

	free(base);
}

static void test_imd_limit_size(void **state)
{
	void *base;
	struct imd imd = {0};
	size_t root_size, max_size;

	max_size = align_up_pow2(sizeof(struct imd_root_pointer) + sizeof(struct imd_root)
				 + 3 * sizeof(struct imd_entry));

	assert_int_equal(-1, imd_limit_size(&imd, max_size));

	base = malloc(LIMIT_ALIGN);
	if (base == NULL)
		fail_msg("Cannot allocate enough memory - fail test");
	imd_handle_init(&imd, (void *)(LIMIT_ALIGN + (uintptr_t)base));

	root_size = align_up_pow2(sizeof(struct imd_root_pointer) + sizeof(struct imd_root)
				  + 2 * sizeof(struct imd_entry));
	imd.lg.r = (void *)imd.lg.limit - root_size;

	imd_create_empty(&imd, root_size, LG_ENTRY_ALIGN);
	assert_int_equal(-1, imd_limit_size(&imd, root_size - 1));
	assert_int_equal(0, imd_limit_size(&imd, max_size));

	/* Cannot create such a big entry */
	assert_null(imd_entry_add(&imd, LG_ENTRY_ID, max_size - root_size + 1));

	free(base);
}

static void test_imd_lockdown(void **state)
{
	struct imd imd = {0};
	struct imd_root *r_lg, *r_sm;

	assert_int_equal(-1, imd_lockdown(&imd));

	imd.lg.r = malloc(sizeof(struct imd_root));
	if (imd.lg.r == NULL)
		fail_msg("Cannot allocate enough memory - fail test");

	r_lg = (struct imd_root *)(imd.lg.r);

	assert_int_equal(0, imd_lockdown(&imd));
	assert_true(r_lg->flags & IMD_FLAG_LOCKED);

	imd.sm.r = malloc(sizeof(struct imd_root));
	if (imd.sm.r == NULL)
		fail_msg("Cannot allocate enough memory - fail test");
	r_sm = (struct imd_root *)(imd.sm.r);

	assert_int_equal(0, imd_lockdown(&imd));
	assert_true(r_sm->flags & IMD_FLAG_LOCKED);

	free(imd.lg.r);
	free(imd.sm.r);
}

static void test_imd_region_used(void **state)
{
	struct imd imd = {0};
	struct imd_entry *first_entry, *new_entry;
	struct imd_root *r;
	size_t size;
	void *imd_base;
	void *base;

	assert_int_equal(-1, imd_region_used(&imd, &base, &size));

	imd_base = malloc(LIMIT_ALIGN);
	if (imd_base == NULL)
		fail_msg("Cannot allocate enough memory - fail test");
	imd_handle_init(&imd, (void *)(LIMIT_ALIGN + (uintptr_t)imd_base));

	assert_int_equal(-1, imd_region_used(&imd, &base, &size));
	assert_int_equal(0, imd_create_empty(&imd, LG_ROOT_SIZE, LG_ENTRY_ALIGN));
	assert_int_equal(0, imd_region_used(&imd, &base, &size));

	r = (struct imd_root *)imd.lg.r;
	first_entry = &r->entries[r->num_entries - 1];

	assert_int_equal(r + first_entry->start_offset, (uintptr_t)base);
	assert_int_equal(first_entry->size, size);

	assert_non_null(imd_entry_add(&imd, LG_ENTRY_ID, LG_ENTRY_SIZE));
	assert_int_equal(2, r->num_entries);

	assert_int_equal(0, imd_region_used(&imd, &base, &size));

	new_entry = &r->entries[r->num_entries - 1];

	assert_true((void *)r + new_entry->start_offset == base);
	assert_int_equal(first_entry->size + new_entry->size, size);

	free(imd_base);
}

static void test_imd_entry_add(void **state)
{
	int i;
	struct imd imd = {0};
	size_t entry_size = 0;
	size_t used_size;
	ssize_t entry_offset;
	void *base;
	struct imd_root *r, *sm_r, *lg_r;
	struct imd_entry *first_entry, *new_entry;
	uint32_t num_entries_copy;
	int32_t max_offset_copy;

	/* No small region case. */
	assert_null(imd_entry_add(&imd, LG_ENTRY_ID, entry_size));

	base = malloc(LIMIT_ALIGN);
	if (base == NULL)
		fail_msg("Cannot allocate enough memory - fail test");

	imd_handle_init(&imd, (void *)(LIMIT_ALIGN + (uintptr_t)base));

	assert_int_equal(0, imd_create_empty(&imd, LG_ROOT_SIZE, LG_ENTRY_ALIGN));

	r = (struct imd_root *)imd.lg.r;
	first_entry = &r->entries[r->num_entries - 1];

	/* Cannot add an entry when root is locked. */
	r->flags = IMD_FLAG_LOCKED;
	assert_null(imd_entry_add(&imd, LG_ENTRY_ID, entry_size));
	r->flags = 0;

	/* Fail when the maximum number of entries has been reached. */
	num_entries_copy = r->num_entries;
	r->num_entries = r->max_entries;
	assert_null(imd_entry_add(&imd, LG_ENTRY_ID, entry_size));
	r->num_entries = num_entries_copy;

	/* Fail when entry size is 0 */
	assert_null(imd_entry_add(&imd, LG_ENTRY_ID, 0));

	/* Fail when entry size (after alignment) overflows imd total size. */
	entry_size = 2049;
	max_offset_copy = r->max_offset;
	r->max_offset = -entry_size;
	assert_null(imd_entry_add(&imd, LG_ENTRY_ID, entry_size));
	r->max_offset = max_offset_copy;

	/* Finally succeed. */
	entry_size = 2 * sizeof(int32_t);
	assert_non_null(imd_entry_add(&imd, LG_ENTRY_ID, entry_size));
	assert_int_equal(2, r->num_entries);

	new_entry = &r->entries[r->num_entries - 1];
	assert_int_equal(sizeof(struct imd_entry), (void *)new_entry - (void *)first_entry);

	assert_int_equal(IMD_ENTRY_MAGIC, new_entry->magic);
	assert_int_equal(LG_ENTRY_ID, new_entry->id);
	assert_int_equal(entry_size, new_entry->size);

	used_size = ALIGN_UP(entry_size, r->entry_align);
	entry_offset = first_entry->start_offset - used_size;
	assert_int_equal(entry_offset, new_entry->start_offset);

	/* Use small region case. */
	imd_create_tiered_empty(&imd, LG_ROOT_SIZE, LG_ENTRY_ALIGN, SM_ROOT_SIZE,
				SM_ENTRY_ALIGN);

	lg_r = imd.lg.r;
	sm_r = imd.sm.r;

	/* All five new entries should be added to small allocations */
	for (i = 0; i < 5; i++) {
		assert_non_null(imd_entry_add(&imd, SM_ENTRY_ID, SM_ENTRY_SIZE));
		assert_int_equal(i + 2, sm_r->num_entries);
		assert_int_equal(2, lg_r->num_entries);
	}

	/* But next should fall back on large region */
	assert_non_null(imd_entry_add(&imd, SM_ENTRY_ID, SM_ENTRY_SIZE));
	assert_int_equal(6, sm_r->num_entries);
	assert_int_equal(3, lg_r->num_entries);

	/*
	 * Small allocation is created when occupies less than 1/4 of available
	 * small region. Verify this.
	 */
	imd_create_tiered_empty(&imd, LG_ROOT_SIZE, LG_ENTRY_ALIGN, SM_ROOT_SIZE,
				SM_ENTRY_ALIGN);

	assert_non_null(imd_entry_add(&imd, SM_ENTRY_ID, -sm_r->max_offset / 4 + 1));
	assert_int_equal(1, sm_r->num_entries);
	assert_int_equal(3, lg_r->num_entries);

	/* Next two should go into small region */
	assert_non_null(imd_entry_add(&imd, SM_ENTRY_ID, -sm_r->max_offset / 4));
	assert_int_equal(2, sm_r->num_entries);
	assert_int_equal(3, lg_r->num_entries);

	/* (1/4 * 3/4) */
	assert_non_null(imd_entry_add(&imd, SM_ENTRY_ID, -sm_r->max_offset / 16 * 3));
	assert_int_equal(3, sm_r->num_entries);
	assert_int_equal(3, lg_r->num_entries);

	free(base);
}

static void test_imd_entry_find(void **state)
{
	struct imd imd = {0};
	void *base;

	base = malloc(LIMIT_ALIGN);
	if (base == NULL)
		fail_msg("Cannot allocate enough memory - fail test");
	imd_handle_init(&imd, (void *)(LIMIT_ALIGN + (uintptr_t)base));

	assert_int_equal(0, imd_create_tiered_empty(&imd, LG_ROOT_SIZE, LG_ENTRY_ALIGN,
						    SM_ROOT_SIZE, SM_ENTRY_ALIGN));

	assert_non_null(imd_entry_add(&imd, LG_ENTRY_ID, LG_ENTRY_SIZE));

	assert_non_null(imd_entry_find(&imd, LG_ENTRY_ID));
	assert_non_null(imd_entry_find(&imd, SMALL_REGION_ID));

	/* Try invalid id, should fail */
	assert_null(imd_entry_find(&imd, INVALID_REGION_ID));

	free(base);
}

static void test_imd_entry_find_or_add(void **state)
{
	struct imd imd = {0};
	const struct imd_entry *entry;
	struct imd_root *r;
	void *base;

	base = malloc(LIMIT_ALIGN);
	if (base == NULL)
		fail_msg("Cannot allocate enough memory - fail test");
	imd_handle_init(&imd, (void *)(LIMIT_ALIGN + (uintptr_t)base));

	assert_null(imd_entry_find_or_add(&imd, LG_ENTRY_ID, LG_ENTRY_SIZE));

	assert_int_equal(0, imd_create_empty(&imd, LG_ROOT_SIZE, LG_ENTRY_ALIGN));
	entry = imd_entry_find_or_add(&imd, LG_ENTRY_ID, LG_ENTRY_SIZE);
	assert_non_null(entry);

	r = (struct imd_root *)imd.lg.r;

	assert_int_equal(entry->id, LG_ENTRY_ID);
	assert_int_equal(2, r->num_entries);
	assert_non_null(imd_entry_find_or_add(&imd, LG_ENTRY_ID, LG_ENTRY_SIZE));
	assert_int_equal(2, r->num_entries);

	free(base);
}

static void test_imd_entry_size(void **state)
{
	struct imd_entry entry = {.size = LG_ENTRY_SIZE};

	assert_int_equal(LG_ENTRY_SIZE, imd_entry_size(&entry));

	entry.size = 0;
	assert_int_equal(0, imd_entry_size(&entry));
}

static void test_imd_entry_at(void **state)
{
	struct imd imd = {0};
	struct imd_root *r;
	struct imd_entry *e = NULL;
	const struct imd_entry *entry;
	void *base;

	base = malloc(LIMIT_ALIGN);
	if (base == NULL)
		fail_msg("Cannot allocate enough memory - fail test");
	imd_handle_init(&imd, (void *)(LIMIT_ALIGN + (uintptr_t)base));

	assert_int_equal(0, imd_create_empty(&imd, LG_ROOT_SIZE, LG_ENTRY_ALIGN));

	/* Fail when entry is NULL */
	assert_null(imd_entry_at(&imd, e));

	entry = imd_entry_add(&imd, LG_ENTRY_ID, LG_ENTRY_SIZE);
	assert_non_null(entry);

	r = (struct imd_root *)imd.lg.r;
	assert_ptr_equal((void *)r + entry->start_offset, imd_entry_at(&imd, entry));

	free(base);
}

static void test_imd_entry_id(void **state)
{
	struct imd_entry entry = {.id = LG_ENTRY_ID};

	assert_int_equal(LG_ENTRY_ID, imd_entry_id(&entry));
}

static void test_imd_entry_remove(void **state)
{
	void *base;
	struct imd imd = {0};
	struct imd_root *r;
	const struct imd_entry *fst_lg_entry, *snd_lg_entry, *fst_sm_entry;
	const struct imd_entry *e = NULL;

	/* Uninitialized handle */
	assert_int_equal(-1, imd_entry_remove(&imd, e));

	base = malloc(LIMIT_ALIGN);
	if (base == NULL)
		fail_msg("Cannot allocate enough memory - fail test");

	imd_handle_init(&imd, (void *)(LIMIT_ALIGN + (uintptr_t)base));

	assert_int_equal(0, imd_create_tiered_empty(&imd, LG_ROOT_SIZE, LG_ENTRY_ALIGN,
						    SM_ROOT_SIZE, SM_ENTRY_ALIGN));

	r = imd.lg.r;
	assert_int_equal(2, r->num_entries);
	fst_lg_entry = &r->entries[0];
	snd_lg_entry = &r->entries[1];

	/* Only last entry can be removed */
	assert_int_equal(-1, imd_entry_remove(&imd, fst_lg_entry));
	r->flags = IMD_FLAG_LOCKED;
	assert_int_equal(-1, imd_entry_remove(&imd, snd_lg_entry));
	r->flags = 0;

	r = imd.sm.r;
	assert_int_equal(1, r->num_entries);
	fst_sm_entry = &r->entries[0];

	/* Fail trying to remove root entry */
	assert_int_equal(-1, imd_entry_remove(&imd, fst_sm_entry));
	assert_int_equal(1, r->num_entries);

	r = imd.lg.r;
	assert_int_equal(0, imd_entry_remove(&imd, snd_lg_entry));
	assert_int_equal(1, r->num_entries);

	/* Fail trying to remove root entry */
	assert_int_equal(-1, imd_entry_remove(&imd, fst_lg_entry));
	assert_int_equal(1, r->num_entries);

	free(base);
}

static void test_imd_cursor_init(void **state)
{
	struct imd imd = {0};
	struct imd_cursor cursor;

	assert_int_equal(-1, imd_cursor_init(NULL, NULL));
	assert_int_equal(-1, imd_cursor_init(NULL, &cursor));
	assert_int_equal(-1, imd_cursor_init(&imd, NULL));
	assert_int_equal(0, imd_cursor_init(&imd, &cursor));

	assert_ptr_equal(cursor.imdr[0], &imd.lg);
	assert_ptr_equal(cursor.imdr[1], &imd.sm);
}

static void test_imd_cursor_next(void **state)
{
	void *base;
	struct imd imd = {0};
	struct imd_cursor cursor;
	struct imd_root *r;
	const struct imd_entry *entry;
	struct imd_entry *fst_lg_entry, *snd_lg_entry, *fst_sm_entry;
	assert_int_equal(0, imd_cursor_init(&imd, &cursor));

	cursor.current_imdr = 3;
	cursor.current_entry = 0;
	assert_null(imd_cursor_next(&cursor));

	cursor.current_imdr = 0;
	assert_null(imd_cursor_next(&cursor));

	base = malloc(LIMIT_ALIGN);
	if (base == NULL)
		fail_msg("Cannot allocate enough memory - fail test");
	imd_handle_init(&imd, (void *)(LIMIT_ALIGN + (uintptr_t)base));

	assert_int_equal(0, imd_create_tiered_empty(&imd, LG_ROOT_SIZE, LG_ENTRY_ALIGN,
						    SM_ROOT_SIZE, SM_ENTRY_ALIGN));

	r = imd.lg.r;
	entry = imd_cursor_next(&cursor);
	assert_non_null(entry);

	fst_lg_entry = &r->entries[0];
	assert_int_equal(fst_lg_entry->id, entry->id);
	assert_ptr_equal(fst_lg_entry, entry);

	entry = imd_cursor_next(&cursor);
	assert_non_null(entry);

	snd_lg_entry = &r->entries[1];
	assert_int_equal(snd_lg_entry->id, entry->id);
	assert_ptr_equal(snd_lg_entry, entry);

	entry = imd_cursor_next(&cursor);
	assert_non_null(entry);

	r = imd.sm.r;
	fst_sm_entry = &r->entries[0];
	assert_int_equal(fst_sm_entry->id, entry->id);
	assert_ptr_equal(fst_sm_entry, entry);

	entry = imd_cursor_next(&cursor);
	assert_null(entry);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_imd_handle_init),
		cmocka_unit_test(test_imd_handle_init_partial_recovery),
		cmocka_unit_test(test_imd_create_empty),
		cmocka_unit_test(test_imd_create_tiered_empty),
		cmocka_unit_test(test_imd_recover),
		cmocka_unit_test(test_imd_limit_size),
		cmocka_unit_test(test_imd_lockdown),
		cmocka_unit_test(test_imd_region_used),
		cmocka_unit_test(test_imd_entry_add),
		cmocka_unit_test(test_imd_entry_find),
		cmocka_unit_test(test_imd_entry_find_or_add),
		cmocka_unit_test(test_imd_entry_size),
		cmocka_unit_test(test_imd_entry_at),
		cmocka_unit_test(test_imd_entry_id),
		cmocka_unit_test(test_imd_entry_remove),
		cmocka_unit_test(test_imd_cursor_init),
		cmocka_unit_test(test_imd_cursor_next),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
