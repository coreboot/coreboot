/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CPU_H
#define _SOC_CPU_H

#include <device/device.h>

#define CPUID_COOPERLAKE_SP_A0			0x05065a

void cpx_init_cpus(struct device *dev);

#endif
