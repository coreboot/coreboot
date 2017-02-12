/*
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#ifndef _MAINBOARD_LENOVO_S230U_EC_H
#define _MAINBOARD_LENOVO_S230U_EC_H

#ifndef __SMM__
void lenovo_s230u_ec_init(void);
#endif

#define ECMM(x) (*((volatile u8 *)(CONFIG_EC_BASE_ADDRESS + x)))
#define ec_mm_read(addr) (ECMM(0x100 + addr))
#define ec_mm_write(addr, data) (ECMM(0x100 + addr) = data)
#define ec_mm_set_bit(addr, bit) (ECMM(0x100 + addr) |= 1 << bit)
#define ec_mm_clr_bit(addr, bit) (ECMM(0x100 + addr) &= ~(1 << bit))

#endif /* _MAINBOARD_LENOVO_S230U_EC_H   */
