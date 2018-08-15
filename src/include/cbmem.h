/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
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

#ifndef _CBMEM_H_
#define _CBMEM_H_

#include <commonlib/cbmem_id.h>
#include <rules.h>
#include <stddef.h>
#include <stdint.h>
#include <boot/coreboot_tables.h>

#define CBMEM_FSP_HOB_PTR	0x614

struct cbmem_entry;

/*
 * The dynamic cbmem infrastructure allows for growing cbmem dynamically as
 * things are added. It requires an external function, cbmem_top(), to be
 * implemented by the board or chipset to define the upper address where
 * cbmem lives. This address is required to be a 32-bit address. Additionally,
 * the address needs to be consistent in both romstage and ramstage.  The
 * dynamic cbmem infrastructure allocates new regions below the last allocated
 * region. Regions are defined by a cbmem_entry struct that is opaque. Regions
 * may be removed, but the last one added is the only that can be removed.
 */

#define DYN_CBMEM_ALIGN_SIZE (4096)
#define CBMEM_ROOT_SIZE      DYN_CBMEM_ALIGN_SIZE

/* The root region is at least DYN_CBMEM_ALIGN_SIZE . */
#define CBMEM_ROOT_MIN_SIZE DYN_CBMEM_ALIGN_SIZE
#define CBMEM_LG_ALIGN CBMEM_ROOT_MIN_SIZE

/* Small allocation parameters. */
#define CBMEM_SM_ROOT_SIZE 1024
#define CBMEM_SM_ALIGN 32

/* Determine the size for CBMEM root and the small allocations */
static inline size_t cbmem_overhead_size(void)
{
	return 2 * CBMEM_ROOT_MIN_SIZE;
}

/* By default cbmem is attempted to be recovered. Returns 0 if cbmem was
 * recovered or 1 if cbmem had to be reinitialized. */
int cbmem_initialize(void);
int cbmem_initialize_id_size(u32 id, u64 size);

/* Initialize cbmem to be empty. */
void cbmem_initialize_empty(void);
void cbmem_initialize_empty_id_size(u32 id, u64 size);

/* Optional hook for platforms to initialize cbmem_top() value. When employed
 * it's called a single time during boot at cbmem initialization/recovery
 * time. */
void cbmem_top_init(void);

/* Return the top address for dynamic cbmem. The address returned needs to
 * be consistent across romstage and ramstage, and it is required to be
 * below 4GiB.
 * x86 boards or chipsets must return NULL before the cbmem backing store has
 * been initialized. */
void *cbmem_top(void);

/* Add a cbmem entry of a given size and id. These return NULL on failure. The
 * add function performs a find first and do not check against the original
 * size. */
const struct cbmem_entry *cbmem_entry_add(u32 id, u64 size);

/* Find a cbmem entry of a given id. These return NULL on failure. */
const struct cbmem_entry *cbmem_entry_find(u32 id);

/* Remove a region defined by a cbmem_entry. Returns 0 on success, < 0 on
 * error. Note: A cbmem_entry cannot be removed unless it was the last one
 * added. */
int cbmem_entry_remove(const struct cbmem_entry *entry);

/* cbmem_entry accessors to get pointer and size of a cbmem_entry. */
void *cbmem_entry_start(const struct cbmem_entry *entry);
u64 cbmem_entry_size(const struct cbmem_entry *entry);

/* Returns 0 if old cbmem was recovered. Recovery is only attempted if
 * s3resume is non-zero. */
int cbmem_recovery(int s3resume);
/* Add a cbmem entry of a given size and id. These return NULL on failure. The
 * add function performs a find first and do not check against the original
 * size. */
void *cbmem_add(u32 id, u64 size);
/* Find a cbmem entry of a given id. These return NULL on failure. */
void *cbmem_find(u32 id);

/* Indicate to each hook if cbmem is being recovered or not. */
typedef void (* const cbmem_init_hook_t)(int is_recovery);
void cbmem_run_init_hooks(int is_recovery);
void cbmem_fail_resume(void);

/* Ramstage only functions. */
/* Add the cbmem memory used to the memory map at boot. */
void cbmem_add_bootmem(void);
/* Return the cbmem memory used */
void cbmem_get_region(void **baseptr, size_t *size);
void cbmem_list(void);
void cbmem_add_records_to_cbtable(struct lb_header *header);

#if ENV_RAMSTAGE
#define ROMSTAGE_CBMEM_INIT_HOOK(init_fn_) __attribute__((unused)) \
	static cbmem_init_hook_t init_fn_ ## _unused_ = init_fn_;
#define RAMSTAGE_CBMEM_INIT_HOOK(init_fn_) \
	static cbmem_init_hook_t init_fn_ ## _ptr_ __attribute__((used, \
	section(".rodata.cbmem_init_hooks"))) = init_fn_;
#define POSTCAR_CBMEM_INIT_HOOK(init_fn_) __attribute__((unused)) \
	static cbmem_init_hook_t init_fn_ ## _unused2_ = init_fn_;
#elif ENV_ROMSTAGE
#define ROMSTAGE_CBMEM_INIT_HOOK(init_fn_) \
	static cbmem_init_hook_t init_fn_ ## _ptr_ __attribute__((used, \
	section(".rodata.cbmem_init_hooks"))) = init_fn_;
#define RAMSTAGE_CBMEM_INIT_HOOK(init_fn_) __attribute__((unused)) \
	static cbmem_init_hook_t init_fn_ ## _unused_ = init_fn_;
#define POSTCAR_CBMEM_INIT_HOOK(init_fn_) __attribute__((unused)) \
	static cbmem_init_hook_t init_fn_ ## _unused2_ = init_fn_;
#elif ENV_POSTCAR
#define ROMSTAGE_CBMEM_INIT_HOOK(init_fn_) __attribute__((unused)) \
	static cbmem_init_hook_t init_fn_ ## _unused2_ = init_fn_;
#define RAMSTAGE_CBMEM_INIT_HOOK(init_fn_) __attribute__((unused)) \
	static cbmem_init_hook_t init_fn_ ## _unused_ = init_fn_;
#define POSTCAR_CBMEM_INIT_HOOK(init_fn_) \
	static cbmem_init_hook_t init_fn_ ## _ptr_ __attribute__((used, \
	section(".rodata.cbmem_init_hooks"))) = init_fn_;
#else
#define ROMSTAGE_CBMEM_INIT_HOOK(init_fn_) __attribute__((unused)) \
	static cbmem_init_hook_t init_fn_ ## _unused_ = init_fn_;
#define RAMSTAGE_CBMEM_INIT_HOOK(init_fn_) __attribute__((unused)) \
	static cbmem_init_hook_t init_fn_ ## _unused2_ = init_fn_;
#define POSTCAR_CBMEM_INIT_HOOK(init_fn_) __attribute__((unused)) \
	static cbmem_init_hook_t init_fn_ ## _unused3_ = init_fn_;
#endif /* ENV_RAMSTAGE */


/* Any new chipset and board must implement cbmem_top() for both
 * romstage and ramstage to support early features like COLLECT_TIMESTAMPS
 * and CBMEM_CONSOLE. Sometimes it is necessary to have cbmem_top()
 * value stored in nvram to enable early recovery on S3 path.
 */
#if IS_ENABLED(CONFIG_ARCH_X86)
/* Note that with LATE_CBMEM_INIT, restore_top_of_low_cacheable()
 * may conditionally return 0 when the sleep type is non S3,
 * i.e. cold and warm boots would return NULL also for cbmem_top. */
void backup_top_of_low_cacheable(uintptr_t ramtop);
uintptr_t restore_top_of_low_cacheable(void);
uintptr_t restore_cbmem_top(void);

/* Deprecated, only use with LATE_CBMEM_INIT. */
void set_late_cbmem_top(uintptr_t ramtop);
#endif

/*
 * Returns 0 for the stages where we know that cbmem does not come online.
 * Even if this function returns 1 for romstage, depending upon the point in
 * bootup, cbmem might not actually be online.
 */
static inline int cbmem_possibly_online(void)
{
	if (ENV_BOOTBLOCK)
		return 0;

	if (ENV_VERSTAGE && IS_ENABLED(CONFIG_VBOOT_STARTS_IN_BOOTBLOCK))
		return 0;

	return 1;
}

#endif /* _CBMEM_H_ */
