/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CPU_INTEL_FSB_H
#define CPU_INTEL_FSB_H

/*
 * This function returns round up 3 * get_timer_fsb()
 */
int get_ia32_fsb_x3(void);

#endif /* CPU_INTEL_FSB_H */
