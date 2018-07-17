/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017-present Facebook, Inc.
 * Copyright 2003-2017    Cavium Inc.  <support@cavium.com>
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

#ifndef __SOC_CAVIUM_CN81XX_SDRAM_H__
#define __SOC_CAVIUM_CN81XX_SDRAM_H__

#include <types.h>

size_t sdram_size_mb(void);
void sdram_init(void);

#endif	/* !__SOC_CAVIUM_CN81XX_SDRAM_H__ */
