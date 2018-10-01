/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
