/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 * Copyright (C) 2015-2016 Intel Corp.
 * Copyright (C) 2017-2018 Siemens AG
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

#ifndef _SOC_BROADWELL_DE_H_
#define _SOC_BROADWELL_DE_H_

#define VTBAR_OFFSET		0x180
#define VTBAR_MASK		0xffffe000
#define VTBAR_ENABLED		0x01
#define VTBAR_SIZE		0x2000

#define SMM_FEATURE_CONTROL	0x58
#define  SMM_CPU_SAVE_EN	(1 << 1)
#define TSEG_BASE		0xa8	/* TSEG base */
#define TSEG_LIMIT		0xac	/* TSEG limit */

/* CPU bus clock is fixed at 100MHz */
#define CPU_BCLK		100

#endif /* _SOC_BROADWELL_DE_H_ */
