/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_NVIDIA_TEGRA210_CCPLEX_H__
#define __SOC_NVIDIA_TEGRA210_CCPLEX_H__

#define MTS_LOAD_ADDRESS 0x82000000

/* Prepare the clocks and rails to start the cpu. */
void ccplex_cpu_prepare(void);

/* Start cpu0 and have it start executing at entry_addr */
void ccplex_cpu_start(void *entry_addr);

#endif /* __SOC_NVIDIA_TEGRA210_CCPLEX_H__ */
