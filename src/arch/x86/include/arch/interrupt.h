/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 * Copyright (C) 2009 Libra Li <libra.li@technexion.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "registers.h"

/* setup interrupt handlers for mainboard */
#if CONFIG_PCI_OPTION_ROM_RUN_REALMODE
extern void mainboard_interrupt_handlers(int intXX, int (*intXX_func)(void));
#elif CONFIG_PCI_OPTION_ROM_RUN_YABEL
#include <device/oprom/yabel/biosemu.h>
#else
static inline void mainboard_interrupt_handlers(int intXX, int (*intXX_func)(void)) { }
#endif

#endif /* INTERRUPT_H */
