/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-or-later */

#ifndef _COMMONLIB_BSD_CBFS_PRIVATE_H_
#define _COMMONLIB_BSD_CBFS_PRIVATE_H_


#include <commonlib/bsd/cb_err.h>
#include <commonlib/bsd/cbfs_mdata.h>
#include <commonlib/bsd/sysincludes.h>
#include <stdbool.h>
#include <stdint.h>
#include <vb2_sha.h>

/*
 * This header implements low-level CBFS access APIs that can be shared across different
 * host applications (e.g. coreboot, libpayload, cbfstool). For verification purposes it
 * implements the metadata hashing part but not the file hashing part, so the host application
 * will need to verify file hashes itself after loading each file. Host applications that use
 * verification should implement wrapper APIs that combine the lookup, loading and hashing steps
 * into a single, safe function call and outside of the code implementing those APIs should not
 * be accessing the low-level APIs in this file directly (e.g. coreboot SoC/driver code should
 * never directly #include this file, and always use the higher level APIs in src/lib/cbfs.c).
 *
 * <cbfs_glue.h> needs to be provided by the host application using this CBFS library. It must
 * define the following type, macros and functions:
 *
 * cbfs_dev_t		An opaque type representing a CBFS storage backend.
 * CBFS_ENABLE_HASHING	Should be 0 to avoid linking hashing features, 1 otherwise. (Only for
 *			metadata hashing. Host application needs to check file hashes itself.)
 * ERROR(...)		printf-style macro to print errors.
 * LOG(...)		printf-style macro to print normal-operation log messages.
 * DEBUG(...)		printf-style macro to print detailed debug output.
 *
 * ssize_t cbfs_dev_read(cbfs_dev_t dev, void *buffer, size_t offset, size_t size);
 *			Read |size| bytes starting at |offset| from |dev| into |buffer|.
 *			Returns amount of bytes read on success and < 0 on error.
 *			This function *MUST* sanity-check offset/size on its own.
 *
 * size_t cbfs_dev_size(cbfs_dev_t dev);
 *			Return the total size in bytes of the CBFS storage (actual CBFS area).
 */
#include <cbfs_glue.h>

/* Flags that modify behavior of cbfs_walk(). */
enum cbfs_walk_flags {
	/* Write the calculated hash back out to |metadata_hash->hash| rather than comparing it.
	   |metadata_hash->algo| must still have been initialized by the caller. */
	CBFS_WALK_WRITEBACK_HASH	= (1 << 0),
	/* Call |walker| for empty file entries (i.e. entries with one of the CBFS_TYPE_DELETED
	   types that mark free space in the CBFS). Otherwise, those entries will be skipped.
	   Either way, these entries are never included in the metadata_hash calculation. */
	CBFS_WALK_INCLUDE_EMPTY		= (1 << 1),
};

/*
 * Traverse a CBFS and call a |walker| callback function for every file. Can additionally
 * calculate a hash over the metadata of all files in the CBFS. If |metadata_hash| is NULL,
 * hashing is disabled. If |walker| is NULL, will just traverse and hash the CBFS without
 * invoking any callbacks (and always return CB_CBFS_NOT_FOUND unless there was another error).
 *
 * |arg| and |dev| will be passed through to |walker| unmodified. |offset| is the absolute
 * offset in |dev| at which the current file metadata starts. |mdata| is a temporary buffer
 * (only valid for the duration of this call to |walker|) containing already read metadata from
 * the current file, up to |already_read| bytes. This will always at least contain the header
 * fields and filename, but may contain more (i.e. attributes), depending on whether hashing is
 * enabled. |walker| should call into cbfs_copy_fill_medadata() to copy the metadata of a file
 * to a persistent buffer and automatically load remaining metadata from |dev| as needed based
 * on the value of |already_read|.
 *
 * |walker| should return CB_CBFS_NOT_FOUND if it wants to continue being called for further
 * files. Any other return code will be used as the final return code for cbfs_walk(). It will
 * return immediately unless it needs to calculate a hash in which case it will still traverse
 * the remaining CBFS (but not call |walker| anymore).
 *
 * Returns, from highest to lowest priority:
 * CB_CBFS_IO		- There was an IO error with the CBFS device (always considered fatal)
 * CB_CBFS_HASH_MISMATCH - |metadata_hash| was provided and did not match the CBFS
 * CB_SUCCESS/<other>	- First non-CB_CBFS_NOT_FOUND code returned by walker()
 * CB_CBFS_NOT_FOUND	- walker() returned CB_CBFS_NOT_FOUND for every file in the CBFS
 */
enum cb_err cbfs_walk(cbfs_dev_t dev, enum cb_err (*walker)(cbfs_dev_t dev, size_t offset,
							    const union cbfs_mdata *mdata,
							    size_t already_read, void *arg),
		      void *arg, struct vb2_hash *metadata_hash, enum cbfs_walk_flags);

/*
 * Helper function that can be used by a |walker| callback to cbfs_walk() to copy the metadata
 * of a file into a permanent buffer. Will copy the |already_read| metadata from |src| into
 * |dst| and load remaining metadata from |dev| as required.
 */
enum cb_err cbfs_copy_fill_metadata(union cbfs_mdata *dst, const union cbfs_mdata *src,
				    size_t already_read, cbfs_dev_t dev, size_t offset);

/* Find a file named |name| in the CBFS on |dev|. Copy its metadata (including attributes)
 * into |mdata_out| and pass out the offset to the file data on the CBFS device.
 * Verify the metadata with |metadata_hash| if provided. */
enum cb_err cbfs_lookup(cbfs_dev_t dev, const char *name, union cbfs_mdata *mdata_out,
			size_t *data_offset_out, struct vb2_hash *metadata_hash);

/* Both base address and size of CBFS mcaches must be aligned to this value! */
#define CBFS_MCACHE_ALIGNMENT	sizeof(uint32_t)	/* Largest data type used in CBFS */

/* Build an in-memory CBFS metadata cache out of the CBFS on |dev| into a |mcache_size| bytes
 * memory area at |mcache|. Also verify |metadata_hash| unless it is NULL. If this returns
 * CB_CBFS_CACHE_FULL, the mcache is still valid and can be used, but lookups may return
 * CB_CBFS_CACHE_FULL for files that didn't fit to indicate that the caller needs to fall back
 * to cbfs_lookup(). */
enum cb_err cbfs_mcache_build(cbfs_dev_t dev, void *mcache, size_t mcache_size,
			      struct vb2_hash *metadata_hash);

/*
 * Find a file named |name| in a CBFS metadata cache and copy its metadata into |mdata_out|.
 * Pass out offset to the file data (on the original CBFS device used for cbfs_mcache_build()).
 */
enum cb_err cbfs_mcache_lookup(const void *mcache, size_t mcache_size, const char *name,
			       union cbfs_mdata *mdata_out, size_t *data_offset_out);

/* Returns the amount of bytes actually used by the CBFS metadata cache in |mcache|. */
size_t cbfs_mcache_real_size(const void *mcache, size_t mcache_size);

#endif	/* _COMMONLIB_BSD_CBFS_PRIVATE_H_ */
