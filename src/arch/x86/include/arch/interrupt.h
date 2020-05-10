/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "registers.h"

/* setup interrupt handlers for mainboard */
#if CONFIG(PCI_OPTION_ROM_RUN_REALMODE)
extern void mainboard_interrupt_handlers(int intXX, int (*intXX_func)(void));
#elif CONFIG(PCI_OPTION_ROM_RUN_YABEL)
#include <device/oprom/yabel/biosemu.h>
#else
static inline void mainboard_interrupt_handlers(int intXX,
	int (*intXX_func)(void)) { }
#endif

#endif /* INTERRUPT_H */
