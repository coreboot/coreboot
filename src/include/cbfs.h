/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CBFS_H_
#define _CBFS_H_

#include <cbmem.h>
#include <commonlib/bsd/cbfs_mdata.h>
#include <commonlib/cbfs.h>
#include <commonlib/mem_pool.h>
#include <program_loading.h>
#include <types.h>
#include <vb2_sha.h>


/**********************************************************************************************
 *                                  CBFS FILE ACCESS APIs                                     *
 **********************************************************************************************/

/*
 * These are the APIs used to access files in CBFS. In order to keep the calls simple and free
 * of clutter in the common cases, but still offer all advanced functionality when needed, there
 * are many different variations that are implemented by wrapping the same underlying API with
 * static inlines. All accessors have in common that they look up files by name, and will
 * transparently decompress files that are compressed.
 *
 * There are three main flavors of CBFS accessors:
 *
 * size_t cbfs_load(char *name, void *buf, size_t size): Loads the contents of a CBFS file into
 *	a buffer provided by the caller (by providing pointer and size to it). Will return the
 *	amount of bytes loaded on success, or 0 on error.
 *
 * void *cbfs_map(char *name, size_t *size_out): Maps a file into the address space. If the file
 *	is not compressed and the platform supports direct memory-mapping for the boot medium,
 *	a pointer to the platform mapping is returned directly. In all other cases, memory will
 *	be allocated from the cbfs_cache and file data will be loaded into there. Returns a
 *	pointer to the mapping on success, or NULL on error. If an optional size_out parameter
 *	is passed in, it will be filled out with the size of the mapped data. Caller should call
 *	cbfs_unmap() after it is done using the mapping to free up the cbfs_cache if possible.
 *
 * void *cbfs_alloc(char *name, cbfs_allocator_t allocator, void *arg, size_t *size_out): Loads
 *	file data into memory provided by a custom allocator function that the caller passes in.
 *	The caller may pass an argument that is passed through verbatim to the allocator.
 *	Returns the pointer returned by the allocator (where the file data was loaded to) on
 *	success, or NULL on error. If an optional size_out parameter is passed in, it will be
 *	filled out with the size of the loaded data.
 *
 * void *cbfs_cbmem_alloc(char *name, uint32_t cbmem_id, size_t *size_out): Wrapper around
 *	cbfs_alloc() that will provide an allocator function for allocating space for the file
 *	data in CBMEM, with the provided CBMEM ID.
 *
 * All of these flavors have variations with any of the following optional parameters added:
 *
 * ..._ro_...: Will force looking up the CBFS file in the read-only CBFS (the "COREBOOT" FMAP
 *	section), even when running in an RW stage from one of the RW CBFSs. Only relevant if
 *	CONFIG(VBOOT) is set.
 *
 * ..._type_...: May pass in an extra enum cbfs_type *type parameter. If the value it points to
 *	is CBFS_TYPE_QUERY, it will be replaced with the actual CBFS type of the found file. If
 *	it is anything else, the type will be compared with the actually found type, and the
 *	operation will fail if they don't match.
 */

/*
 * An allocator function for passing to cbfs_alloc(). Takes the argument that was originally
 * passed to cbfs_alloc(), the size of the file to be loaded, and a pointer to the already
 * loaded and verified file metadata (for rare cases where the allocator needs to check custom
 * attributes). Must return a pointer to space of the requested size where the file data should
 * be loaded, or NULL to make the operation fail.
 */
typedef void *(*cbfs_allocator_t)(void *arg, size_t size, const union cbfs_mdata *mdata);

static inline size_t cbfs_load(const char *name, void *buf, size_t size);
static inline size_t cbfs_ro_load(const char *name, void *buf, size_t size);
static inline size_t cbfs_type_load(const char *name, void *buf, size_t size,
				    enum cbfs_type *type);
static inline size_t cbfs_ro_type_load(const char *name, void *buf, size_t size,
				    enum cbfs_type *type);

static inline void *cbfs_map(const char *name, size_t *size_out);
static inline void *cbfs_ro_map(const char *name, size_t *size_out);
static inline void *cbfs_type_map(const char *name, size_t *size_out, enum cbfs_type *type);
static inline void *cbfs_ro_type_map(const char *name, size_t *size_out, enum cbfs_type *type);

static inline void *cbfs_alloc(const char *name, cbfs_allocator_t allocator, void *arg,
			       size_t *size_out);
static inline void *cbfs_ro_alloc(const char *name, cbfs_allocator_t allocator, void *arg,
				  size_t *size_out);
static inline void *cbfs_type_alloc(const char *name, cbfs_allocator_t allocator, void *arg,
				    size_t *size_out, enum cbfs_type *type);
static inline void *cbfs_ro_type_alloc(const char *name, cbfs_allocator_t allocator, void *arg,
				       size_t *size_out, enum cbfs_type *type);

static inline void *cbfs_cbmem_alloc(const char *name, uint32_t cbmem_id, size_t *size_out);
static inline void *cbfs_ro_cbmem_alloc(const char *name, uint32_t cbmem_id, size_t *size_out);
static inline void *cbfs_type_cbmem_alloc(const char *name, uint32_t cbmem_id, size_t *size_out,
					  enum cbfs_type *type);
static inline void *cbfs_ro_type_cbmem_alloc(const char *name, uint32_t cbmem_id,
					     size_t *size_out, enum cbfs_type *type);

/* Removes a previously allocated CBFS mapping. Should try to unmap mappings in strict LIFO
   order where possible, since mapping backends often don't support more complicated cases. */
void cbfs_unmap(void *mapping);

/* Load stage into memory filling in prog. Return 0 on success. < 0 on error. */
int cbfs_prog_stage_load(struct prog *prog);


/**********************************************************************************************
 *                                BOOT DEVICE HELPER APIs                                     *
 **********************************************************************************************/

/*
 * The shared memory pool for backing mapped CBFS files, and other CBFS allocation needs.
 * On x86 platforms, this would only be needed to transparently map compressed files, but it
 * would require a permanent CBMEM carveout to be safe to use during S3 resume. Since it's not
 * clear whether this feature is necessary or worth the wasted memory, it is currently disabled
 * but could be added behind a Kconfig later if desired.
 */
#define CBFS_CACHE_AVAILABLE (!CONFIG(ARCH_X86))
extern struct mem_pool cbfs_cache;

/*
 * Data structure that represents "a" CBFS boot device, with optional metadata cache. Generally
 * we only have one of these, or two (RO and RW) when CONFIG(VBOOT) is set. The region device
 * stored here must always be a subregion of boot_device_ro().
 */
struct cbfs_boot_device {
	struct region_device rdev;
	void *mcache;
	size_t mcache_size;
};

/* Helper to fill out |mcache| and |mcache_size| in a cbfs_boot_device. */
void cbfs_boot_device_find_mcache(struct cbfs_boot_device *cbd, uint32_t id);

/*
 * Retrieves the currently active CBFS boot device. If |force_ro| is set, will always return the
 * read-only CBFS instead (this only makes a difference when CONFIG(VBOOT) is enabled). May
 * perform certain CBFS initialization tasks. Returns NULL on error (e.g. boot device IO error).
 */
const struct cbfs_boot_device *cbfs_get_boot_device(bool force_ro);

/*
 * Builds the mcache (if |cbd->mcache| is set) and verifies |metadata_hash| (if it is not NULL).
 * If CB_CBFS_CACHE_FULL is returned, the mcache is incomplete but still valid and the metadata
 * hash was still verified. Should be called once per *boot* (not once per stage) before the
 * first CBFS access.
 */
cb_err_t cbfs_init_boot_device(const struct cbfs_boot_device *cbd,
			       struct vb2_hash *metadata_hash);


/**********************************************************************************************
 *                         LEGACY APIs, TO BE DEPRECATED/REPLACED                             *
 **********************************************************************************************/

/* Locate file by name and optional type. Return 0 on success. < 0 on error. */
int cbfs_boot_locate(struct cbfsf *fh, const char *name, uint32_t *type);
/* Locate file in a specific region of fmap. Return 0 on success. < 0 on error*/
int cbfs_locate_file_in_region(struct cbfsf *fh, const char *region_name,
		const char *name, uint32_t *type);

/* Return mapping of option ROM found in boot device. NULL on error. */
void *cbfs_boot_map_optionrom(uint16_t vendor, uint16_t device);
/* Return mapping of option ROM with revision number. Returns NULL on error. */
void *cbfs_boot_map_optionrom_revision(uint16_t vendor, uint16_t device, uint8_t rev);


/**********************************************************************************************
 *                         INTERNAL HELPERS FOR INLINES, DO NOT USE.                          *
 **********************************************************************************************/
void *_cbfs_alloc(const char *name, cbfs_allocator_t allocator, void *arg,
		  size_t *size_out, bool force_ro, enum cbfs_type *type);

struct _cbfs_default_allocator_arg {
	void *buf;
	size_t buf_size;
};
void *_cbfs_default_allocator(void *arg, size_t size, const union cbfs_mdata *unused);

void *_cbfs_cbmem_allocator(void *arg, size_t size, const union cbfs_mdata *unused);

/**********************************************************************************************
 *                                  INLINE IMPLEMENTATIONS                                    *
 **********************************************************************************************/
static inline void *cbfs_alloc(const char *name, cbfs_allocator_t allocator, void *arg,
			       size_t *size_out)
{
	return cbfs_type_alloc(name, allocator, arg, size_out, NULL);
}

static inline void *cbfs_ro_alloc(const char *name, cbfs_allocator_t allocator, void *arg,
				  size_t *size_out)
{
	return cbfs_ro_type_alloc(name, allocator, arg, size_out, NULL);
}

static inline void *cbfs_type_alloc(const char *name, cbfs_allocator_t allocator, void *arg,
				    size_t *size_out, enum cbfs_type *type)
{
	return _cbfs_alloc(name, allocator, arg, size_out, false, type);
}

static inline void *cbfs_ro_type_alloc(const char *name, cbfs_allocator_t allocator, void *arg,
				       size_t *size_out, enum cbfs_type *type)
{
	return _cbfs_alloc(name, allocator, arg, size_out, true, type);
}

static inline void *cbfs_map(const char *name, size_t *size_out)
{
	return cbfs_type_map(name, size_out, NULL);
}

static inline void *cbfs_ro_map(const char *name, size_t *size_out)
{
	return cbfs_ro_type_map(name, size_out, NULL);
}

static inline void *cbfs_type_map(const char *name, size_t *size_out, enum cbfs_type *type)
{
	return cbfs_type_alloc(name, NULL, NULL, size_out, type);
}

static inline void *cbfs_ro_type_map(const char *name, size_t *size_out, enum cbfs_type *type)
{
	return cbfs_ro_type_alloc(name, NULL, NULL, size_out, type);
}

static inline size_t _cbfs_load(const char *name, void *buf, size_t size, bool force_ro,
				enum cbfs_type *type)
{
	struct _cbfs_default_allocator_arg arg = { .buf = buf, .buf_size = size };
	if (_cbfs_alloc(name, _cbfs_default_allocator, &arg, &size, force_ro, type))
		return size;
	else
		return 0;
}

static inline size_t cbfs_load(const char *name, void *buf, size_t size)
{
	return cbfs_type_load(name, buf, size, NULL);
}

static inline size_t cbfs_type_load(const char *name, void *buf, size_t size,
				    enum cbfs_type *type)
{
	return _cbfs_load(name, buf, size, false, type);
}

static inline size_t cbfs_ro_load(const char *name, void *buf, size_t size)
{
	return cbfs_ro_type_load(name, buf, size, NULL);
}

static inline size_t cbfs_ro_type_load(const char *name, void *buf, size_t size,
				    enum cbfs_type *type)
{
	return _cbfs_load(name, buf, size, true, type);
}

static inline void *cbfs_cbmem_alloc(const char *name, uint32_t cbmem_id, size_t *size_out)
{
	return cbfs_type_cbmem_alloc(name, cbmem_id, size_out, NULL);
}

static inline void *cbfs_ro_cbmem_alloc(const char *name, uint32_t cbmem_id, size_t *size_out)
{
	return cbfs_ro_type_cbmem_alloc(name, cbmem_id, size_out, NULL);
}

static inline void *cbfs_type_cbmem_alloc(const char *name, uint32_t cbmem_id, size_t *size_out,
					  enum cbfs_type *type)
{
	return cbfs_type_alloc(name, _cbfs_cbmem_allocator, (void *)(uintptr_t)cbmem_id,
			       size_out, type);
}

static inline void *cbfs_ro_type_cbmem_alloc(const char *name, uint32_t cbmem_id,
					     size_t *size_out, enum cbfs_type *type)
{
	return cbfs_ro_type_alloc(name, _cbfs_cbmem_allocator, (void *)(uintptr_t)cbmem_id,
				  size_out, type);
}

#endif
