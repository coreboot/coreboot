/*
 * This file is part of the coreboot project.
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

#ifndef CPU_INTEL_FSB_H
#define CPU_INTEL_FSB_H

/*
 * This function returns round up 3 * get_timer_fsb()
 */
int get_ia32_fsb_x3(void);

#endif /* CPU_INTEL_FSB_H */
