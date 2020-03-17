/*
 * This file is part of the coreboot project.
 *
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

#ifndef RAMINIT_H
#define RAMINIT_H

/* The 440BX supports up to four (single- or double-sided) DIMMs. */
#define DIMM_SOCKETS	4

void enable_spd(void);
void disable_spd(void);
void sdram_initialize(void);
void mainboard_enable_serial(void);

/* Debug */
#if CONFIG(DEBUG_RAM_SETUP)
void dump_spd_registers(void);
void dump_pci_device(unsigned int dev);
#else
#define dump_spd_registers()
#endif
#endif				/* RAMINIT_H */
