/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CBFS_H_
#define _CBFS_H_

#include <commonlib/cbfs.h>
#include <program_loading.h>

/***********************************************
 * Perform CBFS operations on the boot device. *
 ***********************************************/

/* Return mapping of option ROM found in boot device. NULL on error. */
void *cbfs_boot_map_optionrom(uint16_t vendor, uint16_t device);
/* Return mapping of option ROM with revision number. Returns NULL on error. */
void *cbfs_boot_map_optionrom_revision(uint16_t vendor, uint16_t device, uint8_t rev);
/* Locate file by name and optional type. Return 0 on success. < 0 on error. */
int cbfs_boot_locate(struct cbfsf *fh, const char *name, uint32_t *type);
/* Map file into memory leaking the mapping. Only should be used when
 * leaking mappings are a no-op. Returns NULL on error, else returns
 * the mapping and sets the size of the file. */
void *cbfs_boot_map_with_leak(const char *name, uint32_t type, size_t *size);
/* Locate file in a specific region of fmap. Return 0 on success. < 0 on error*/
int cbfs_locate_file_in_region(struct cbfsf *fh, const char *region_name,
		const char *name, uint32_t *type);
/* Load an arbitrary type file from CBFS into a buffer. Returns amount of
 * loaded bytes on success or 0 on error. File will get decompressed as
 * necessary.  Same decompression requirements as
 * cbfs_load_and_decompress(). */
size_t cbfs_boot_load_file(const char *name, void *buf, size_t buf_size,
	uint32_t type);
/* Load |in_size| bytes from |rdev| at |offset| to the |buffer_size| bytes
 * large |buffer|, decompressing it according to |compression| in the process.
 * Returns the decompressed file size, or 0 on error.
 * LZMA files will be mapped for decompression. LZ4 files will be decompressed
 * in-place with the buffer size requirements outlined in compression.h. */
size_t cbfs_load_and_decompress(const struct region_device *rdev, size_t offset,
	size_t in_size, void *buffer, size_t buffer_size, uint32_t compression);

/* Load stage into memory filling in prog. Return 0 on success. < 0 on error. */
int cbfs_prog_stage_load(struct prog *prog);

/* Returns the region device of the currently active CBFS.
   Return < 0 on error, 0 on success. */
int cbfs_boot_region_device(struct region_device *rdev);

#endif
