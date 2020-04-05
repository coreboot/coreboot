/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _SOC_APOLLOLAKE_ITSS_H_
#define _SOC_APOLLOLAKE_ITSS_H_

#define GPIO_IRQ_START	50
#define GPIO_IRQ_END	ITSS_MAX_IRQ

#define ITSS_MAX_IRQ	119
#define IRQS_PER_IPC	32
#define NUM_IPC_REGS	((ITSS_MAX_IRQ + IRQS_PER_IPC - 1)/IRQS_PER_IPC)

#endif /* _SOC_APOLLOLAKE_ITSS_H_ */
