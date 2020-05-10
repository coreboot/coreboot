/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_STAGES_H
#define __ARCH_STAGES_H

#include <main_decl.h>

void stage_entry(int hart_id, void *fdt)
	__attribute__((section(".text.stage_entry")));

#endif
