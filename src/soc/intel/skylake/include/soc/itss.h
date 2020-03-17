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

#ifndef SOC_INTEL_SKL_ITSS_H
#define SOC_INTEL_SKL_ITSS_H

#define GPIO_IRQ_START	50
#define GPIO_IRQ_END	ITSS_MAX_IRQ

#define ITSS_MAX_IRQ	119
#define IRQS_PER_IPC	32
#define NUM_IPC_REGS	((ITSS_MAX_IRQ + IRQS_PER_IPC - 1)/IRQS_PER_IPC)

#endif	/* SOC_INTEL_SKL_ITSS_H */
