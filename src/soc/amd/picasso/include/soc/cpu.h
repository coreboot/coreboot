/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __PICASSO_CPU_H__
#define __PICASSO_CPU_H__

#include <device/device.h>

#define CSTATE_BASE_REG 0xc0010073

void picasso_init_cpus(struct device *dev);
int get_cpu_count(void);
void check_mca(void);

#define PICASSO_CPUID			0x00810f81
#define RAVEN2_CPUID			0x00820f01

#endif /* __PICASSO_CPU_H__ */
