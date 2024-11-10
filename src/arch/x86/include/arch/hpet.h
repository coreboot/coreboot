/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ARCH_X86_HPET_H
#define ARCH_X86_HPET_H

#define HPET_BASE_ADDRESS	0xfed00000

/**
 * Definitions are from IA-PC HPET (High Precision Event Timers) Specification,
 * Revision 1.0a
 */
#define HPET_GEN_CAP_ID	0x0
#define  HPET_NUM_TIM_CAP_MASK	0x1f
#define  HPET_NUM_TIM_CAP_SHIFT	8

#define HPET_TMR0_CNF_CAP	0x100
#define  HPET_TIMER_FSB_EN_CNF_MASK	(1 << 15)

#endif /* ARCH_X86_HPET_H */
