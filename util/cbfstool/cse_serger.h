/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __CBFSTOOL_CSE_SERGER_H__
#define __CBFSTOOL_CSE_SERGER_H__

#include <commonlib/endian.h>
#include <commonlib/region.h>

#include "common.h"
#include "cse_helpers.h"

#define BPDT_SIGNATURE			(0x000055AA)

#define BUFF_SIZE_ALIGN			(4 * KiB)

enum bpdt_version {
	BPDT_VERSION_1_6 = 1,
	BPDT_VERSION_1_7 = 2,
};

enum subpart_hdr_version {
	SUBPART_HDR_VERSION_1 = 1,
	SUBPART_HDR_VERSION_2 = 2,
};

enum subpart_entry_version {
	SUBPART_ENTRY_VERSION_1 = 1,
};

struct cse_layout_regions {
	struct region data_partition;
	struct region bp1;
	struct region bp2;
	struct region bp3;
	struct region bp4;
};

typedef void *cse_layout_ptr;
typedef void *bpdt_hdr_ptr;
typedef void *subpart_hdr_ptr;

struct bpdt_entry {
	uint32_t type;
	uint32_t offset;
	uint32_t size;
} __packed;

struct bpdt_ops {
	bool (*match_version)(const struct buffer *buff);

	bpdt_hdr_ptr (*create_hdr)(void);
	void (*print_hdr)(const bpdt_hdr_ptr ptr);
	bpdt_hdr_ptr (*read_hdr)(struct buffer *buff);
	int (*write_hdr)(struct buffer *buff, const bpdt_hdr_ptr ptr);

	size_t (*get_entry_count)(const bpdt_hdr_ptr ptr);
	void (*inc_entry_count)(bpdt_hdr_ptr ptr);

	cse_layout_ptr (*create_layout)(const struct cse_layout_regions *regions);
	void (*print_layout)(const cse_layout_ptr ptr);
	cse_layout_ptr (*read_layout)(struct buffer *buff);
	int (*write_layout)(struct buffer *buff, const cse_layout_ptr ptr);

	void (*update_checksum)(bpdt_hdr_ptr ptr, struct bpdt_entry *entry);
	bool (*validate_checksum)(bpdt_hdr_ptr ptr, struct bpdt_entry *entry);

	enum subpart_hdr_version subpart_hdr_version;
	enum subpart_entry_version subpart_entry_version;
};

struct subpart_hdr_ops {
	subpart_hdr_ptr (*read)(struct buffer *buffer);
	void (*print)(const subpart_hdr_ptr ptr);
	size_t (*get_entry_count)(const subpart_hdr_ptr ptr);
	void (*free)(subpart_hdr_ptr ptr);
};

struct subpart_entry_ops {
	void (*print)(struct buffer *buff, size_t size);
};

extern const struct bpdt_ops bpdt_1_7_ops;
extern const struct bpdt_ops bpdt_1_6_ops;

extern const struct subpart_hdr_ops subpart_hdr_1_ops;
extern const struct subpart_hdr_ops subpart_hdr_2_ops;

extern const struct subpart_entry_ops subpart_entry_1_ops;

#endif /* __CBFSTOOL_CSE_SERGER_H__ */
