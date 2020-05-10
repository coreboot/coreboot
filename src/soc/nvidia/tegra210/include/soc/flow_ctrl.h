/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _TEGRA210_FLOW_CTRL_H_
#define _TEGRA210_FLOW_CTRL_H_

#include <stdint.h>

void flowctrl_cpu_off(int cpu);
void flowctrl_cpu_on(int cpu);
void flowctrl_cpu_suspend(int cpu);
void flowctrl_write_cc4_ctrl(int cpu, uint32_t val);
void flowctrl_write_cpu_csr(int cpu, uint32_t val);
void flowctrl_write_cpu_halt(int cpu, uint32_t val);

#endif
