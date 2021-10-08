/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __CBFSTOOL_CSE_FPT_H__
#define __CBFSTOOL_CSE_FPT_H__

#include <commonlib/endian.h>
#include <commonlib/region.h>

#include "common.h"
#include "cse_helpers.h"

#define FPT_MARKER			"$FPT"
#define FPT_ENTRY_VERSION		0x10

enum fpt_hdr_version {
	FPT_HDR_VERSION_20 = 0x20,
	FPT_HDR_VERSION_21 = 0x21,
};

typedef void *fpt_hdr_ptr;

struct fpt_hdr_ops {
	bool (*match_version)(struct buffer *buff);

	fpt_hdr_ptr (*read)(struct buffer *buff);
	void (*print)(const fpt_hdr_ptr ptr);

	size_t (*get_entry_count)(const fpt_hdr_ptr ptr);
};

extern const struct fpt_hdr_ops fpt_hdr_20_ops;
extern const struct fpt_hdr_ops fpt_hdr_21_ops;

#endif /* __CBFSTOOL_CSE_FPT_H__ */
