/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>

/*
 * At runtime TXE creates the FIT table in the shared SRAM and patches the bootblock
 * at the fixed address 4G - 64 byte with a pointer to this FIT table. In order to be able
 * to pre-compute the PCR value for the bootblock this FIT pointer needs to be added to the
 * image as well. Since the FIT location is fixed in TXE, this can be done at build time.
 */
__attribute__((used, __section__(".fit_pointer"))) const uint64_t fit_ptr = 0xfffe0000;
