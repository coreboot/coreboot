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

#ifndef __ARCH_SYMBOLS_H
#define __ARCH_SYMBOLS_H

/*
 * The _car_region_[start|end] covers the entirety of the cache as RAM
 * region. All other symbols with the _car prefix a subsets of this
 * larger region.
 */
extern char _car_region_start[];
extern char _car_region_end[];
#define _car_region_size (_car_region_end - _car_region_start)

/*
 * This is the stack area used for all stages that execute when cache-as-ram
 * is up. Area is not cleared in between stages.
 */
extern char _car_stack[];
extern char _ecar_stack[];
#define _car_stack_size (_ecar_stack - _car_stack)

extern char _car_unallocated_start[];

extern char _car_ehci_dbg_info[];
extern char _ecar_ehci_dbg_info[];
#define _car_ehci_dbg_info_size \
	(_ecar_ehci_dbg_info - _car_ehci_dbg_info)

#endif
