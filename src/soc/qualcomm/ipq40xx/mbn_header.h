/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QCA_IPQ40XX_MBN_HEADER_H__
#define __SOC_QCA_IPQ40XX_MBN_HEADER_H__

#include <types.h>

/* QCA firmware blob header gleaned from util/qualcomm/ipqheader.py */

struct mbn_header {
	u32	mbn_type;
	u32	mbn_version;
	u32	mbn_source;
	u32	mbn_destination;
	u32	mbn_total_size;
	u32	mbn_padding[5];
};

#endif
