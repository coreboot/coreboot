/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef CPLD_H
#define CPLD_H

unsigned int cpld_read_pcb_version(void);
void cpld_reset_bridge(void);

#endif
