/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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

#ifndef _SOC_MSR_H_
#define _SOC_MSR_H_

#define MSR_IA32_PLATFORM_ID    0x17
#define MSR_CORE_THREAD_COUNT   0x35
#define MSR_PLATFORM_INFO       0xce
#define MSR_TURBO_RATIO_LIMIT   0x1ad
#define MSR_IA32_MC0_STATUS     0x400
#define MSR_PKG_POWER_SKU_UNIT  0x606
#define MSR_PKG_POWER_LIMIT     0x610

#endif /* _SOC_MSR_H_ */
