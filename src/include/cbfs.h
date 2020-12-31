/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CBFS_H_
#define _CBFS_H_

#include <cbmem.h>
#include <commonlib/cbfs.h>
#include <commonlib/mem_pool.h>
#include <program_loading.h>
#include <types.h>
#include <vb2_sha.h>


/**********************************************************************************************
 *                                  CBFS FILE ACCESS APIs                                     *
 **********************************************************************************************/

/* Map file into memory, returning a pointer to the mapping or NULL on error. If |size_out| is
   not NULL, it will pass out the size of the mapped file.
   NOTE: Since this may return a direct pointer to memory-mapped hardware, compressed files are
   NOT transparently decompressed (unlike cbfs_load()). */
static inline void *cbfs_map(const char *name, size_t *size_out);

/* Like cbfs_map(), except that it will always read from the read-only CBFS (the "COREBOOT" FMAP
   region), even when CONFIG(VBOOT) is enabled. */
static inline void *cbfs_ro_map(const char *name, size_t *size_out);

/* Removes a previously allocated CBFS mapping. Should try to unmap mappings in strict LIFO
   order where possible, since mapping backends often don't support more complicated cases. */
void cbfs_unmap(void *mapping);

/* Load a file from CBFS into a buffer. Returns amount of loaded bytes on success or 0 on error.
   File will get decompressed as necessary. */
static inline size_t cbfs_load(const char *name, void *buf, size_t buf_size);
/* Like cbfs_load(), except that it will always read from the read-only CBFS (the "COREBOOT"
   FMAP region), even when CONFIG(VBOOT) is enabled. */
static inline size_t cbfs_ro_load(const char *name, void *buf, size_t buf_size);

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

/* Load |in_size| bytes from |rdev| at |offset| to the |buffer_size| bytes large |buffer|,
   decompressing it according to |compression| in the process. Returns the decompressed file
   size, or 0 on error. LZMA files will be mapped for decompression. LZ4 files will be
   decompressed in-place with the buffer size requirements outlined in compression.h. */
size_t cbfs_load_and_decompress(const struct region_device *rdev, size_t offset,
	size_t in_size, void *buffer, size_t buffer_size, uint32_t compression);


/**********************************************************************************************
 *                         INTERNAL HELPERS FOR INLINES, DO NOT USE.                          *
 **********************************************************************************************/
size_t _cbfs_load(const char *name, void *buf, size_t buf_size, bool force_ro);
void *_cbfs_map(const char *name, size_t *size_out, bool force_ro);


/**********************************************************************************************
 *                                  INLINE IMPLEMENTATIONS                                    *
 **********************************************************************************************/
static inline void *cbfs_map(const char *name, size_t *size_out)
{
	return _cbfs_map(name, size_out, false);
}

static inline void *cbfs_ro_map(const char *name, size_t *size_out)
{
	return _cbfs_map(name, size_out, true);
}

static inline size_t cbfs_load(const char *name, void *buf, size_t buf_size)
{
	return _cbfs_load(name, buf, buf_size, false);
}

static inline size_t cbfs_ro_load(const char *name, void *buf, size_t buf_size)
{
	return _cbfs_load(name, buf, buf_size, true);
}

#endif
