/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0-or-later */

#ifndef _COMMONLIB_BSD_CBFS_MDATA_H_
#define _COMMONLIB_BSD_CBFS_MDATA_H_

#include <commonlib/bsd/cbfs_serialized.h>
#include <stddef.h>
#include <stdint.h>

/*
 * Helper structure to allocate space for a blob of metadata on the stack.
 * NOTE: The fields in any union cbfs_mdata or any of its substructures from cbfs_serialized.h
 * should always remain in the same byte order as they are stored on flash (= big endian). To
 * avoid byte-order confusion, fields should always and only be converted to host byte order at
 * exactly the time they are read from one of these structures into their own separate variable.
 */
union cbfs_mdata {
	struct cbfs_file h;
	uint8_t raw[CBFS_METADATA_MAX_SIZE];
};

/* Finds a CBFS attribute in a metadata block. Attribute returned as-is (still big-endian).
   If |size| is not 0, will check that it matches the length of the attribute (if found)...
   else caller is responsible for checking the |len| field to avoid reading out-of-bounds. */
const void *cbfs_find_attr(const union cbfs_mdata *mdata, uint32_t attr_tag, size_t size_check);

#endif	/* _COMMONLIB_BSD_CBFS_MDATA_H_ */
