/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SOC_QUALCOMM_IPQ806X_MBN_HEADER_H__
#define __SOC_QUALCOMM_IPQ806X_MBN_HEADER_H__

#include <types.h>

/* Qualcomm firmware blob header gleaned from util/ipqheader/ipqheader.py */

struct mbn_header {
	u32	mbn_type;
	u32	mbn_version;
	u32	mbn_source;
	u32	mbn_destination;
	u32	mbn_total_size;
	u32	mbn_padding[5];
};

#endif
