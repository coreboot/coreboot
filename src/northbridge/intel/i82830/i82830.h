/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Joseph Smith <joe@smittys.pointclark.net>
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

#define RRBAR   0x48 /* Register Range Base Address (0x00000000) */
#define GCC0    0x50 /* GMCH Control #0 (0xa072) */
#define GCC1    0x52 /* GMCH Control #1 (0x0000) */
#define FDHC    0x58 /* Fixed DRAM Hole Control (0x00) */
#define PAM0    0x59 /* Programable Attribute Map #0 (0x00) */
#define PAM1    0x5a /* Programable Attribute Map #1 (0x00) */
#define PAM2    0x5b /* Programable Attribute Map #2 (0x00) */
#define PAM3    0x5c /* Programable Attribute Map #3 (0x00) */
#define PAM4    0x5d /* Programable Attribute Map #4 (0x00) */
#define PAM5    0x5e /* Programable Attribute Map #5 (0x00) */
#define PAM6    0x5f /* Programable Attribute Map #6 (0x00) */
#define DRB     0x60 /* DRAM Row Boundary #0 (0x00) */
#define DRB1    0x61 /* DRAM Row Boundary #1 (0x00) */
#define DRB2    0x62 /* DRAM Row Boundary #2 (0x00) */
#define DRB3    0x63 /* DRAM Row Boundary #3 (0x00) */
#define DRA     0x70 /* DRAM Row Attribute #0 (0xff) */
#define DRA1    0x71 /* DRAM Row Attribute #1 (0xff) */
#define DRT     0x78 /* DRAM Timing (0x00000010) */
#define DRC     0x7c /* DRAM Controller Mode #0 (0x00000000) */
#define DRC1    0x7d /* DRAM Controller Mode #1 (0x00000000) */
#define DRC2    0x7e /* DRAM Controller Mode #2 (0x00000000) */
#define DRC3    0x7f /* DRAM Controller Mode #3 (0x00000000) */
#define DTC     0x8c /* DRAM Throttling Control (0x00000000) */
#define SMRAM   0x90 /* System Management RAM Control (0x02) */
#define ESMRAMC 0x91 /* Extended System Management RAM Control Reg. (0x38) */
#define ERRSTS  0x92 /* Error Status (0x0000) */
#define ERRCMD  0x94 /* Error Command (0x0000) */
#define BUFF_SC 0xec /* System Memory Buffer Strength Control (0x00000000) */
#define APBASE  0x10 /* Aperture Base Configuration (0x00000008) */
#define APSIZE  0xb4 /* Apterture Size (0x00) */
#define ATTBASE 0xb8 /* Aperture Translation Table Base (0x00000000) */

