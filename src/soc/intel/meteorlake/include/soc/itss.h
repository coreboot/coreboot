/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_MTL_ITSS_H
#define SOC_INTEL_MTL_ITSS_H

#define GPIO_IRQ_START	50
#define GPIO_IRQ_END	ITSS_MAX_IRQ

#define ITSS_MAX_IRQ	119
#define IRQS_PER_IPC	32
#define NUM_IPC_REGS	((ITSS_MAX_IRQ + IRQS_PER_IPC - 1)/IRQS_PER_IPC)

#endif	/* SOC_INTEL_ADL_ITSS_H */
