/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_STAGES_H
#define __ARCH_STAGES_H

#include <stdint.h>
#include <main_decl.h>

void stage_entry(uintptr_t stage_arg);

/* This function is the romstage platform entry point, and should contain all
   chipset and mainboard setup until DRAM is initialized and accessible. */
void platform_romstage_main(void);
/* This is an optional hook to run further chipset or mainboard code after DRAM
   and associated support frameworks (like CBMEM) have been initialized. */
void platform_romstage_postram(void);

#endif
