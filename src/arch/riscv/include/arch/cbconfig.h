/* SPDX-License-Identifier: GPL-2.0-only */

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
