/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _IMD_PRIVATE_H_
#define _IMD_PRIVATE_H_

#include <cbmem.h>
#include <commonlib/bsd/helpers.h>

/* In-memory data structures. */
struct imd_root_pointer {
	uint32_t magic;
	/* Relative to upper limit/offset. */
	int32_t root_offset;
} __packed;

struct imd_entry {
	uint32_t magic;
	/* start is located relative to imd_root */
	int32_t start_offset;
	uint32_t size;
	uint32_t id;
} __packed;

struct imd_root {
	uint32_t max_entries;
	uint32_t num_entries;
	uint32_t flags;
	uint32_t entry_align;
	/* Used for fixing the size of an imd. Relative to the root. */
	int32_t max_offset;
	struct imd_entry entries[0];
} __packed;

#define IMD_ROOT_PTR_MAGIC  0xc0389481
#define IMD_ENTRY_MAGIC  (~0xc0389481)
#define SMALL_REGION_ID  CBMEM_ID_IMD_SMALL
#define LIMIT_ALIGN 4096

#define IMD_FLAG_LOCKED 1

#endif /* _IMD_PRIVATE_H */
