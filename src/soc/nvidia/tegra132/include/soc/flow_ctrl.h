/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2014, NVIDIA CORPORATION.  All rights reserved.
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

#ifndef _TEGRA132_FLOW_CTRL_H_
#define _TEGRA132_FLOW_CTRL_H_

void flowctrl_cpu_off(int cpu);
void flowctrl_write_cpu_halt(int cpu, uint32_t val);

#endif
