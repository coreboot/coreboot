/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * Copyright 2016 Google Inc.
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
 * This is the stack used under CONFIG_C_ENVIRONMENT_BOOTBLOCK for
 * all stages that execute when cache-as-ram is up.
 */
extern char _car_stack_start[];
extern char _car_stack_end[];
#define _car_stack_size (_car_stack_end - _car_stack_start)

extern char _car_ehci_dbg_info_start[];
extern char _car_ehci_dbg_info_end[];
#define _car_ehci_dbg_info_size \
	(_car_ehci_dbg_info_end - _car_ehci_dbg_info_start)

/*
 * The _car_relocatable_data_[start|end] symbols cover CAR data which is
 * relocatable once memory comes online. Variables with CAR_GLOBAL decoration
 * reside within this region. The _car_global_[start|end] is a subset of the
 * relocatable region which excludes the timestamp region because of
 * intricacies in the timestamp code.
 */
extern char _car_relocatable_data_start[];
extern char _car_relocatable_data_end[];
#define _car_relocatable_data_size \
	(_car_relocatable_data_end - _car_relocatable_data_start)
extern char _car_global_start[];
extern char _car_global_end[];
#define _car_global_size (_car_global_end - _car_global_start)

#endif
