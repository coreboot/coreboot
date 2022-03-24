/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_XEON_SP_ITSS_H
#define SOC_INTEL_XEON_SP_ITSS_H

#define ITSS_MAX_IRQ	119
#define IRQS_PER_IPC	32
#define NUM_IPC_REGS	((ITSS_MAX_IRQ + IRQS_PER_IPC - 1)/IRQS_PER_IPC)

#endif	/* SOC_INTEL_XEON_SP_ITSS_H */
