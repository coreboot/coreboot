/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MICROCHIP_MEC152X_EC_H
#define MICROCHIP_MEC152X_EC_H

/*
 * Used references: MEC152x datasheet (Microchip document DS00003427C)
 */

#include <types.h>

/* TABLE 3-1: BASE ADDRESS */
#define LDN_ESPI_IO_COMPONENT	0xd

/* TABLE 9-6: ESPI I/O BASE ADDRESS REGISTER DEFAULT VALUES */
#define EMI_0_IOBASE_INDEX	0x68
#define EMI_1_IOBASE_INDEX	0x6c

void ec_espi_io_program_iobase(const u16 port, const u8 iobase_index, const u16 base);

void ec_emi_read(u8 *dest, const u16 base, const u8 region, const u16 offset, const u16 length);

#endif /* MICROCHIP_MEC152X_EC_H */
