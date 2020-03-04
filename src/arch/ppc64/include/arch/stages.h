/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __ARCH_STAGES_H
#define __ARCH_STAGES_H

#include <main_decl.h>

void stage_entry(uintptr_t stage_arg) __attribute__((section(".text.stage_entry")));

#endif
