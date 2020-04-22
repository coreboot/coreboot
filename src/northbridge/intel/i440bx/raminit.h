/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef RAMINIT_H
#define RAMINIT_H

/* The 440BX supports up to four (single- or double-sided) DIMMs. */
#define DIMM_SOCKETS	4

void enable_spd(void);
void disable_spd(void);
void sdram_initialize(int s3resume);

/* Debug */
#if CONFIG(DEBUG_RAM_SETUP)
void dump_spd_registers(void);
void dump_pci_device(unsigned int dev);
#else
#define dump_spd_registers()
#endif
#endif				/* RAMINIT_H */
