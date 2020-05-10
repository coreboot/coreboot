/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QUALCOMM_IPQ806X_MBN_HEADER_H__
#define __SOC_QUALCOMM_IPQ806X_MBN_HEADER_H__

#include <types.h>

/* Qualcomm firmware blob header gleaned from util/qualcomm/ipqheader.py */

struct mbn_header {
	u32	mbn_type;
	u32	mbn_version;
	u32	mbn_source;
	u32	mbn_destination;
	u32	mbn_total_size;
	u32	mbn_padding[5];
};

#endif
