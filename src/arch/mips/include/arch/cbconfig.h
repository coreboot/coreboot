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

#ifndef _ARCH_CBCONFIG_H_
#define _ARCH_CBCONFIG_H_

/*
 * Instead of using Kconfig variables for internal coreboot infrastructure
 * variables that are architecture dependent land those things in this file.
 * If it's not obvious all variables that are used in the common code need
 * to have the same name across all architectures.
 */

#define COREBOOT_TABLE_SIZE 0x2000

#endif
