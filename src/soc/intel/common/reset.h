/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _INTEL_COMMON_RESET_H_
#define _INTEL_COMMON_RESET_H_

/*
 * Implement SoC specific global reset (i.e. a reset of both host and
 * ME partitions). Usually the ME is asked to perform the reset first.
 * If that doesn't work out, fall back to a manual global reset.
 */
void do_global_reset(void);

/* Prepare for reset, run do_global_reset(), halt. */
__noreturn void global_reset(void);

#endif	/* _INTEL_COMMON_RESET_H_ */
