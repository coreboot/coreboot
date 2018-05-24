/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation
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

#ifndef _SOC_SKYLAKE_BOOTBLOCK_H_
#define _SOC_SKYLAKE_BOOTBLOCK_H_

#include <intelblocks/systemagent.h>

#if IS_ENABLED(CONFIG_PLATFORM_USES_FSP1_1)
#include <fsp/bootblock.h>
#else
static inline void bootblock_fsp_temp_ram_init(void) {}
#endif

/* Bootblock pre console init programming */
void bootblock_cpu_init(void);
void bootblock_pch_early_init(void);

/* Bootblock post console init programming */
void i2c_early_init(void);
void pch_early_init(void);
void pch_early_iorange_init(void);
void report_platform_info(void);
void report_memory_config(void);

#endif
