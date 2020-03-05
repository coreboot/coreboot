/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CBFS_H_
#define _CBFS_H_

#include <cbmem.h>
#include <commonlib/cbfs.h>
#include <program_loading.h>
#include <types.h>

/***********************************************
 * Perform CBFS operations on the boot device. *
 ***********************************************/

/* Return mapping of option ROM found in boot device. NULL on error. */
void *cbfs_boot_map_optionrom(uint16_t vendor, uint16_t device);
/* Return mapping of option ROM with revision number. Returns NULL on error. */
void *cbfs_boot_map_optionrom_revision(uint16_t vendor, uint16_t device, uint8_t rev);
/* Locate file by name and optional type. Return 0 on success. < 0 on error. */
int cbfs_boot_locate(struct cbfsf *fh, const char *name, uint32_t *type);
/* Map file into memory, returning a pointer to the mapping or NULL on error.
   If |size_out| is not NULL, it will pass out the size of the mapped file.
   NOTE: Since this may return a direct pointer to memory-mapped hardware,
   compressed files are NOT transparently decompressed (unlike cbfs_load()). */
void *cbfs_map(const char *name, size_t *size_out);
/* Removes a mapping previously allocated with cbfs_map(). Should try to unmap
   mappings in strict LIFO order where possible, since mapping backends often
   don't support more complicated cases. */
int cbfs_unmap(void *mapping);
/* Locate file in a specific region of fmap. Return 0 on success. < 0 on error*/
int cbfs_locate_file_in_region(struct cbfsf *fh, const char *region_name,
		const char *name, uint32_t *type);
/* Load a file from CBFS into a buffer. Returns amount of loaded bytes on
   success or 0 on error. File will get decompressed as necessary. Same
   decompression requirements as cbfs_load_and_decompress(). */
size_t cbfs_load(const char *name, void *buf, size_t buf_size);
/* Load |in_size| bytes from |rdev| at |offset| to the |buffer_size| bytes
 * large |buffer|, decompressing it according to |compression| in the process.
 * Returns the decompressed file size, or 0 on error.
 * LZMA files will be mapped for decompression. LZ4 files will be decompressed
 * in-place with the buffer size requirements outlined in compression.h. */
size_t cbfs_load_and_decompress(const struct region_device *rdev, size_t offset,
	size_t in_size, void *buffer, size_t buffer_size, uint32_t compression);

/* Load stage into memory filling in prog. Return 0 on success. < 0 on error. */
int cbfs_prog_stage_load(struct prog *prog);

/*
 * Data structure that represents "a" CBFS boot device, with optional metadata
 * cache. Generally we only have one of these, or two (RO and RW) when
 * CONFIG(VBOOT) is set. The region device stored here must always be a
 * subregion of boot_device_ro().
 */
struct cbfs_boot_device {
	struct region_device rdev;
	void *mcache;
	size_t mcache_size;
};

/* Helper to fill out |mcache| and |mcache_size| in a cbfs_boot_device. */
void cbfs_boot_device_find_mcache(struct cbfs_boot_device *cbd, uint32_t id);

/*
 * Retrieves the currently active CBFS boot device. If |force_ro| is set, will
 * always return the read-only CBFS instead (this only makes a difference when
 * CONFIG(VBOOT) is enabled). May perform certain CBFS initialization tasks.
 * Returns NULL on error (e.g. boot device IO error).
 */
const struct cbfs_boot_device *cbfs_get_boot_device(bool force_ro);

#endif
