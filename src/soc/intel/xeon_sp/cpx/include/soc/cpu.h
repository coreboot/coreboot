/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CPU_H
#define _SOC_CPU_H

#include <device/device.h>
#include <cpu/intel/cpu_ids.h>
#include <cpu/x86/msr.h>

void cpx_init_cpus(struct device *dev);

#endif
