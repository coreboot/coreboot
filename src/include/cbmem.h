/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CBMEM_H_
#define _CBMEM_H_

#include <commonlib/bsd/cbmem_id.h> /* IWYU pragma: export */
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

/* Return the top address for dynamic cbmem. The address returned needs to
 * be consistent across romstage and ramstage, and it is required to be
 * below 4GiB for 32bit coreboot builds. On 64bit coreboot builds there's no
 * upper limit. This should not be called before memory is initialized.
 */
/* The assumption is made that the result of cbmem_top_romstage fits in the size
   of uintptr_t in the ramstage. */
extern uintptr_t _cbmem_top_ptr;
void *cbmem_top(void);
/* With CONFIG_RAMSTAGE_CBMEM_TOP_ARG set, the result of cbmem_top is passed via
 * calling arguments to the next stage and saved in the global _cbmem_top_ptr
 * global variable. Only a romstage callback needs to be implemented by the
 * platform. It is up to the stages after romstage to save the calling argument
 * in the _cbmem_top_ptr symbol. Without CONFIG_RAMSTAGE_CBMEM_TOP_ARG the same
 * implementation as used in romstage will be used.
 */
void *cbmem_top_chipset(void);

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

/* Ramstage only functions. */
/* Add the cbmem memory used to the memory map at boot. */
void cbmem_add_bootmem(void);
/* Return the cbmem memory used */
void cbmem_get_region(void **baseptr, size_t *size);
void cbmem_list(void);
void cbmem_add_records_to_cbtable(struct lb_header *header);

#define _CBMEM_INIT_HOOK_UNUSED(init_fn_) __attribute__((unused)) \
	static cbmem_init_hook_t init_fn_ ## _unused_ = init_fn_

#define _CBMEM_INIT_HOOK(init_fn_) \
	static cbmem_init_hook_t init_fn_ ## _ptr_ __attribute__((used, \
	section(".rodata.cbmem_init_hooks"))) = init_fn_

/* Early hooks get executed before other hooks. Use sparingly for hooks that create
   CBMEM regions which need to remain in a constant location across boot modes. */
#define _CBMEM_INIT_HOOK_EARLY(init_fn_) \
	static cbmem_init_hook_t init_fn_ ## _ptr_ __attribute__((used, \
	section(".rodata.cbmem_init_hooks_early"))) = init_fn_

/* Use CBMEM_CREATION_HOOK for code that needs to be run when cbmem is initialized
   for the first time. */
#if ENV_CREATES_CBMEM
#define CBMEM_CREATION_HOOK(x)		_CBMEM_INIT_HOOK(x)
#else
#define CBMEM_CREATION_HOOK(x)		_CBMEM_INIT_HOOK_UNUSED(x)
#endif

/* Use CBMEM_READY_HOOK for code that needs to run in all stages that have cbmem. */
#if ENV_HAS_CBMEM
#define CBMEM_READY_HOOK(x)		_CBMEM_INIT_HOOK(x)
#define CBMEM_READY_HOOK_EARLY(x)	_CBMEM_INIT_HOOK_EARLY(x)
#else
#define CBMEM_READY_HOOK(x)		_CBMEM_INIT_HOOK_UNUSED(x)
#define CBMEM_READY_HOOK_EARLY(x)	_CBMEM_INIT_HOOK_UNUSED(x)
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

	if (ENV_SEPARATE_VERSTAGE && !CONFIG(VBOOT_STARTS_IN_ROMSTAGE))
		return 0;

	return 1;
}

/* Returns 1 after running cbmem init hooks, 0 otherwise. */
static inline int cbmem_online(void)
{
	extern int cbmem_initialized;

	if (!cbmem_possibly_online())
		return 0;

	return cbmem_initialized;
}

#endif /* _CBMEM_H_ */
