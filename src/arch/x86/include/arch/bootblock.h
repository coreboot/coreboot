/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __ARCH_BOOTBLOCK_H__
#define __ARCH_BOOTBLOCK_H__

void bootblock_early_cpu_init(void);
void bootblock_early_northbridge_init(void);
void bootblock_early_southbridge_init(void);

#endif
