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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef __SOC_NVIDIA_TEGRA210_CCPLEX_H__
#define __SOC_NVIDIA_TEGRA210_CCPLEX_H__

#include <stdint.h>

#define MTS_LOAD_ADDRESS 0x82000000

/* Prepare the clocks and rails to start the cpu. */
void ccplex_cpu_prepare(void);

/* Start cpu0 and have it start executing at entry_addr */
void ccplex_cpu_start(void *entry_addr);

#endif /* __SOC_NVIDIA_TEGRA210_CCPLEX_H__ */
