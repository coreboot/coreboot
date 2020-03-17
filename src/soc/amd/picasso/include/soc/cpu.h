/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __PICASSO_CPU_H__
#define __PICASSO_CPU_H__

#include <device/device.h>

#define CSTATE_BASE_REG 0xc0010073

void picasso_init_cpus(struct device *dev);
int get_cpu_count(void);
void check_mca(void);

#endif /* __PICASSO_CPU_H__ */
