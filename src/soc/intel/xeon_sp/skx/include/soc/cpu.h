/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_CPU_H_
#define _SOC_CPU_H_

#include <device/device.h>
#include <cpu/intel/cpu_ids.h>
#include <cpu/x86/msr.h>

/* CPU bus clock is fixed at 100MHz */
#define CPU_BCLK 100

void xeon_sp_init_cpus(struct device *dev);

#endif
