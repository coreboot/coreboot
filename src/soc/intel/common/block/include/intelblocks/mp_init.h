/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_MP_INIT_H
#define SOC_INTEL_COMMON_BLOCK_MP_INIT_H

#include <cpu/intel/cpu_ids.h>
#include <device/device.h>

/* Supported CPUIDs for different SOCs */
/*
 * MP Init callback function to Find CPU Topology. This function is common
 * among all SOCs and thus its in Common CPU block.
 */
int get_cpu_count(void);

/*
 * MP Init callback function(get_microcode_info) to find the Microcode at
 * Pre MP Init phase. This function is common among all SOCs and thus its in
 * Common CPU block.
 * This function also fills in the microcode patch (in *microcode), and also
 * sets the argument *parallel to 1, which allows microcode loading in all
 * APs to occur in parallel during MP Init.
 */
void get_microcode_info(const void **microcode, int *parallel);

/*
 * This function will perform any recommended CPU (BSP and AP) initialization
 * after coreboot has done the multiprocessor initialization (before FSP-S)
 * and prior to coreboot perform post_cpus_init (after DRAM resources are set).
 */
void before_post_cpus_init(void);

/*
 * SoC Overrides
 *
 * All new SoC must implement below functionality for ramstage.
 */

/*
 * In this function SOC must perform CPU feature programming
 * during Ramstage phase.
 */
void soc_core_init(struct device *dev);

#endif	/* SOC_INTEL_COMMON_BLOCK_MP_INIT_H */
