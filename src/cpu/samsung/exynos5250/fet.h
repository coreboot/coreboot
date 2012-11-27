/*
 * Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 */

#ifndef __ASM_ARM_ARCH_EXYNOS5_FET_H
#define __ASM_ARM_ARCH_EXYNOS5_FET_H

/* The FET IDs for TPS65090 PMU chip. */
enum {
	FET_ID_BL = 1
	FET_ID_VIDEO,
	FET_ID_WWAN,
	FET_ID_SDCARD,
	FET_ID_CAMOUT,
	FET_ID_LCD,
	FET_ID_TS
};

#endif
