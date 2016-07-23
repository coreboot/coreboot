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

/* Bootblock pre console init programing */
void bootblock_cpu_init(void);
void bootblock_pch_early_init(void);
void bootblock_systemagent_early_init(void);

void pch_uart_init(void);
/* Bootblock post console init programing */
void pch_enable_lpc(void);

#endif
