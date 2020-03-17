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

#ifndef RAMINIT_TABLES_H
#define RAMINIT_TABLES_H

#include <types.h>

/* [CHANNEL][EXT_REVISON][LANE][2*SLOT+RANK][CLOCK_SPEED] */
extern const u8 u8_FFFD1240[2][5][9][4][4];

extern const u16 u16_FFFE0EB8[2][4];

/* [CARD][LANE][CLOCK_SPEED] */
extern const u16 u16_ffd1188[2][9][4];

/* [REVISION][CHANNEL][CLOCK_INDEX][?] */
extern const u8 u8_FFFD1891[2][2][4][12];

extern const u8 u8_FFFD17E0[2][5][4][4];

extern const u8 u8_FFFD0C78[2][5][4][2][2][4];

extern const u16 u16_fffd0c68[3];

extern const u16 u16_fffd0c70[2][2];

extern const u16 u16_fffd0c50[3][2][2];

/* [CLOCK_INDEX] */
extern const u16 min_cycletime[4];

/* [CLOCK_INDEX] */
extern const u16 min_cas_latency_time[4];

/* [CHANNEL][EXT_SILICON_REVISION][?][CLOCK_INDEX] */
/* On other mobos may also depend on slot and rank.  */
extern const u8 u8_FFFD0EF8[2][5][4][4];

/* [CLOCK_SPEED] */
extern const u8 u8_FFFD1218[4];

extern const u8 reg178_min[];
extern const u8 reg178_max[];
extern const u8 reg178_step[];

extern const u16 u16_ffd1178[2][4];

extern const u16 u16_fe0eb8[2][4];

extern const u8 lut16[4];

#endif // RAMINIT_TABLES_H
