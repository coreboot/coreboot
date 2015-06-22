/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2014-2015, NVIDIA CORPORATION.  All rights reserved.
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

#ifndef _TEGRA210_FLOW_CTRL_H_
#define _TEGRA210_FLOW_CTRL_H_

void flowctrl_cpu_off(int cpu);
void flowctrl_cpu_on(int cpu);
void flowctrl_cpu_suspend(int cpu);
void flowctrl_write_cc4_ctrl(int cpu, uint32_t val);
void flowctrl_write_cpu_csr(int cpu, uint32_t val);
void flowctrl_write_cpu_halt(int cpu, uint32_t val);

#endif
