/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017-present Facebook, Inc.
 * Copyright 2003-2017  Cavium Inc.  <support@cavium.com>
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

#ifndef SRC_SOC_CAVIUM_CN81XX_INCLUDE_CLOCK_H_
#define SRC_SOC_CAVIUM_CN81XX_INCLUDE_CLOCK_H_

#include <types.h>

u64 thunderx_get_ref_clock(void);
u64 thunderx_get_io_clock(void);
u64 thunderx_get_core_clock(void);

#endif /* SRC_SOC_CAVIUM_CN81XX_INCLUDE_CLOCK_H_ */
