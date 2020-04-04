/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __SOC_NVIDIA_TEGRA210_CPU_H__
#define __SOC_NVIDIA_TEGRA210_CPU_H__

/*
 * Start a core in 64-bit mode at the entry_64 address. Note that entry_64
 * should be a 32-bit address.
 */
void start_cpu(int cpu, void *entry_64);
/* Start CPU wthout any log messages. */
void start_cpu_silent(int cpu, void *entry_64);
/* Prepare SoC for starting a CPU. Initialize the global state of the SoC. */
void cpu_prepare_startup(void *entry_64);

#endif /* __SOC_NVIDIA_TEGRA210_CPU_H__ */
