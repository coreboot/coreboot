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
 * Perform BSP and AP initialization
 * This function can be called in below cases
 * 1. During coreboot is doing MP initialization as part of BS_DEV_INIT_CHIPS (exclude
 * this call if user has selected USE_INTEL_FSP_MP_INIT)
 * 2. coreboot would like to take APs control back after FSP-S has done with MP
 * initialization based on user select USE_INTEL_FSP_MP_INIT
 */
void init_cpus(void);

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

/*
 * In this function SOC must fill required mp_ops params, also it
 * should call these mp_ops callback functions by calling
 * mp_init_with_smm() function from x86/mp_init.c file.
 *
 * Also, if there is any other SOC specific functionalities to be
 * implemented before or after MP Init, it can be done here.
 */
void soc_init_cpus(struct bus *cpu_bus);

#endif	/* SOC_INTEL_COMMON_BLOCK_MP_INIT_H */
