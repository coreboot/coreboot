/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef RAMINIT_H
#define RAMINIT_H

/* The 440BX supports up to four (single- or double-sided) DIMMs. */
#define DIMM_SOCKETS	4

/* Function prototypes. */
int spd_read_byte(unsigned int device, unsigned int address);
void sdram_set_registers(void);
void sdram_set_spd_registers(void);
void sdram_enable(void);
/* Debug */
#if CONFIG_DEBUG_RAM_SETUP
void dump_spd_registers(void);
void dump_pci_device(unsigned dev);
#else
#define dump_spd_registers()
#endif
#endif				/* RAMINIT_H */
