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
 * but WITHOUT ANY WARRANTY; without even the implied wacbmem_entryanty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <bootstate.h>
#include <boot/tables.h>
#include <console/console.h>
#include <cbmem.h>
#include <string.h>
#include <stdlib.h>
#include <arch/early_variables.h>
#if CONFIG_HAVE_ACPI_RESUME && !defined(__PRE_RAM__)
#include <arch/acpi.h>
#endif

#ifndef UINT_MAX
#define UINT_MAX 4294967295U
#endif

/* ACPI resume needs to be cleared in the fail-to-recover case, but that
 * condition is only handled during ramstage. */
#if CONFIG_HAVE_ACPI_RESUME && !defined(__PRE_RAM__)
static inline void cbmem_handle_acpi_resume(void)
{
	/* Something went wrong, our high memory area got wiped */
	if (acpi_slp_type == 3 || acpi_slp_type == 2)
		acpi_slp_type = 0;
}
#else
static inline void cbmem_handle_acpi_resume(void) {}
#endif

/*
 * The dynamic cbmem code uses a root region. The root region boundary
 * addresses are determined by cbmem_top() and ROOT_MIN_SIZE. Just below
 * the address returned by cbmem_top() is a pointer that points to the
 * root data structure. The root data structure provides the book keeping
 * for each large entry.
 */

/* The root region is at least DYN_CBMEM_ALIGN_SIZE . */
#define ROOT_MIN_SIZE DYN_CBMEM_ALIGN_SIZE
#define CBMEM_POINTER_MAGIC 0xc0389479
#define CBMEM_ENTRY_MAGIC ~(CBMEM_POINTER_MAGIC)

/* The cbmem_root_pointer structure lives just below address returned
 * from cbmem_top(). It points to the root data structure that
 * maintains the entries. */
struct cbmem_root_pointer {
	u32 magic;
	u32 root;
} __attribute__((packed));

struct cbmem_entry {
	u32 magic;
	u32 start;
	u32 size;
	u32 id;
} __attribute__((packed));

struct cbmem_root {
	u32 max_entries;
	u32 num_entries;
	u32 locked;
	u32 size;
	struct cbmem_entry entries[0];
} __attribute__((packed));


static inline void *cbmem_top_cached(void)
{
#if !defined(__PRE_RAM__)
	static void *cached_cbmem_top;

	if (cached_cbmem_top == NULL)
		cached_cbmem_top = cbmem_top();

	return cached_cbmem_top;
#else
	return cbmem_top();
#endif
}

static inline void *get_top_aligned(void)
{
	unsigned long top;

	/* Align down what is returned from cbmem_top(). */
	top = (unsigned long)cbmem_top_cached();
	top &= ~(DYN_CBMEM_ALIGN_SIZE - 1);

	return (void *)top;
}

static inline void *get_root(void)
{
	unsigned long pointer_addr;
	struct cbmem_root_pointer *pointer;

	pointer_addr = (unsigned long)get_top_aligned();
	pointer_addr -= sizeof(struct cbmem_root_pointer);

	pointer = (void *)pointer_addr;
	if (pointer->magic != CBMEM_POINTER_MAGIC)
		return NULL;

	return (void *)pointer->root;
}

static inline void cbmem_entry_assign(struct cbmem_entry *entry,
                                      u32 id, u32 start, u32 size)
{
	entry->magic = CBMEM_ENTRY_MAGIC;
	entry->start = start;
	entry->size = size;
	entry->id = id;
}

static inline const struct cbmem_entry *
cbmem_entry_append(struct cbmem_root *root, u32 id, u32 start, u32 size)
{
	struct cbmem_entry *cbmem_entry;

	cbmem_entry = &root->entries[root->num_entries];
	root->num_entries++;

	cbmem_entry_assign(cbmem_entry, id, start, size);

	return cbmem_entry;
}

void cbmem_initialize_empty(void)
{
	unsigned long pointer_addr;
	unsigned long root_addr;
	unsigned long max_entries;
	struct cbmem_root *root;
	struct cbmem_root_pointer *pointer;

	/* Place the root pointer and the root. The number of entries is
	 * dictated by difference between the root address and the pointer
	 * where the root address is aligned down to
	 * DYN_CBMEM_ALIGN_SIZE. The pointer falls just below the
	 * address returned by get_top_aligned(). */
	pointer_addr = (unsigned long)get_top_aligned();
	root_addr = pointer_addr - ROOT_MIN_SIZE;
	root_addr &= ~(DYN_CBMEM_ALIGN_SIZE - 1);
	pointer_addr -= sizeof(struct cbmem_root_pointer);

	max_entries = (pointer_addr - (root_addr + sizeof(*root))) /
	              sizeof(struct cbmem_entry);

	pointer = (void *)pointer_addr;
	pointer->magic = CBMEM_POINTER_MAGIC;
	pointer->root = root_addr;

	root = (void *)root_addr;
	root->max_entries = max_entries;
	root->num_entries = 0;
	root->locked = 0;
	root->size = pointer_addr - root_addr +
	             sizeof(struct cbmem_root_pointer);

	/* Add an entry covering the root region. */
	cbmem_entry_append(root, CBMEM_ID_ROOT, root_addr, root->size);

	printk(BIOS_DEBUG, "CBMEM: root @ %p %d entries.\n",
	       root, root->max_entries);

	cbmem_arch_init();

	/* Migrate cache-as-ram variables. */
	car_migrate_variables();
}

static inline int cbmem_fail_recovery(void)
{
	cbmem_initialize_empty();
	cbmem_handle_acpi_resume();
	/* Migrate cache-as-ram variables. */
	car_migrate_variables();
	return 1;
}

static int validate_entries(struct cbmem_root *root)
{
	unsigned int i;
	u32 current_end;

	current_end = (u32)get_top_aligned();

	printk(BIOS_DEBUG, "CBMEM: recovering %d/%d entries from root @ %p\n",
	       root->num_entries, root->max_entries, root);

	/* Check that all regions are properly aligned and are just below
	 * the previous entry */
	for (i = 0; i < root->num_entries; i++) {
		struct cbmem_entry *entry = &root->entries[i];

		if (entry->magic != CBMEM_ENTRY_MAGIC)
			return -1;

		if (entry->start & (DYN_CBMEM_ALIGN_SIZE - 1))
			return -1;

		if (entry->start + entry->size != current_end)
			return -1;

		current_end = entry->start;
	}

	return 0;
}

int cbmem_initialize(void)
{
	struct cbmem_root *root;
	void *top_according_to_root;

	root = get_root();

	/* No recovery possible since root couldn't be recovered. */
	if (root == NULL)
		return cbmem_fail_recovery();

	/* Sanity check the root. */
	top_according_to_root = (void *)(root->size + (unsigned long)root);
	if (get_top_aligned() != top_according_to_root)
		return cbmem_fail_recovery();

	if (root->num_entries > root->max_entries)
		return cbmem_fail_recovery();

	if ((root->max_entries * sizeof(struct cbmem_entry)) >
	    (root->size - sizeof(struct cbmem_root_pointer) - sizeof(*root)))
		return cbmem_fail_recovery();

	/* Validate current entries. */
	if (validate_entries(root))
		return cbmem_fail_recovery();

#if defined(__PRE_RAM__)
	/* Lock the root in the romstage on a recovery. The assumption is that
	 * recovery is called during romstage on the S3 resume path. */
	root->locked = 1;
#endif

	cbmem_arch_init();

	/* Migrate cache-as-ram variables. */
	car_migrate_variables();

	/* Recovery successful. */
	return 0;
}

static void *cbmem_base(void)
{
	struct cbmem_root *root;
	u32 low_addr;

	root = get_root();

	if (root == NULL)
		return NULL;

	low_addr = (u32)root;

	/* Assume the lowest address is the last one added. */
	if (root->num_entries > 0) {
		low_addr = root->entries[root->num_entries - 1].start;
	}

	return (void *)low_addr;
}


const struct cbmem_entry *cbmem_entry_add(u32 id, u64 size64)
{
	struct cbmem_root *root;
	const struct cbmem_entry *entry;
	unsigned long base;;
	u32 size;
	u32 aligned_size;

	entry = cbmem_entry_find(id);

	if (entry != NULL)
		return entry;

	/* Only handle sizes <= UINT_MAX internally. */
	if (size64 > (u64)UINT_MAX)
		return NULL;

	size = size64;

	root = get_root();

	if (root == NULL)
		return NULL;

	/* Nothing can be added once it is locked down. */
	if (root->locked)
		return NULL;

	if (root->max_entries == root->num_entries)
		return NULL;

	aligned_size = ALIGN(size, DYN_CBMEM_ALIGN_SIZE);
	base = (unsigned long)cbmem_base();
	base -= aligned_size;

	return cbmem_entry_append(root, id, base, aligned_size);
}

void *cbmem_add(u32 id, u64 size)
{
	const struct cbmem_entry *entry;

	entry = cbmem_entry_add(id, size);

	if (entry == NULL)
		return NULL;

	return cbmem_entry_start(entry);
}

/* Retrieve a region provided a given id. */
const struct cbmem_entry *cbmem_entry_find(u32 id)
{
	struct cbmem_root *root;
	const struct cbmem_entry *entry;
	unsigned int i;

	root = get_root();

	if (root == NULL)
		return NULL;

	entry = NULL;

	for (i = 0; i < root->num_entries; i++) {
		if (root->entries[i].id == id) {
			entry = &root->entries[i];
			break;
		}
	}

	return entry;
}

void *cbmem_find(u32 id)
{
	const struct cbmem_entry *entry;

	entry = cbmem_entry_find(id);

	if (entry == NULL)
		return NULL;

	return cbmem_entry_start(entry);
}

/* Remove a reserved region. Returns 0 on success, < 0 on error. Note: A region
 * cannot be removed unless it was the last one added. */
int cbmem_entry_remove(const struct cbmem_entry *entry)
{
	unsigned long entry_num;
	struct cbmem_root *root;

	root = get_root();

	if (root == NULL)
		return -1;

	if (root->num_entries == 0)
		return -1;

	/* Nothing can be removed. */
	if (root->locked)
		return -1;

	entry_num = entry - &root->entries[0];

	/* If the entry is the last one in the root it can be removed. */
	if (entry_num == (root->num_entries - 1)) {
		root->num_entries--;
		return 0;
	}

	return -1;
}

u64 cbmem_entry_size(const struct cbmem_entry *entry)
{
	return entry->size;
}

void *cbmem_entry_start(const struct cbmem_entry *entry)
{
	return (void *)entry->start;
}


#if !defined(__PRE_RAM__)
/* selected cbmem can be initialized early in ramstage. Additionally, that
 * means cbmem console can be reinitialized early as well. The post_device
 * function is empty since cbmem was initialized early in ramstage. */
static void init_cbmem_pre_device(void *unused)
{
	cbmem_initialize();
#if CONFIG_CONSOLE_CBMEM
	cbmemc_reinit();
#endif /* CONFIG_CONSOLE_CBMEM */
}

BOOT_STATE_INIT_ENTRIES(cbmem_bscb) = {
	BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY,
	                      init_cbmem_pre_device, NULL),
};

void cbmem_add_lb_mem(struct lb_memory *mem)
{
	unsigned long base;
	unsigned long top;

	base = (unsigned long)cbmem_base();
	top = (unsigned long)get_top_aligned();
	lb_add_memory_range(mem, LB_MEM_TABLE, base, top - base);
}

void cbmem_list(void)
{
	unsigned int i;
	struct cbmem_root *root;

	root = get_root();

	if (root == NULL)
		return;

	for (i = 0; i < root->num_entries; i++) {
		struct cbmem_entry *entry;

		entry = &root->entries[i];

		cbmem_print_entry(i, entry->id, entry->start, entry->size);
	}
}
#endif /* __PRE_RAM__ */
