/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2009 Maciej Pijanka <maciej.pijanka@gmail.com>
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

/*
 * Datasheet:
 *   - Name: Intel 440LX AGPset: 82443LX Host Bridge/Controller
 *   - PDF: 29056402.pdf
 */

/*
 * Host-to-PCI Bridge Registers.
 * The values in parenthesis are the default values as per datasheet.
 * Any addresses between 0x00 and 0xff not listed below are either
 *
 * i didnt listed every register that IS implemented, just usefull ones
 *  -- Maciej `agaran` Pijanka
 *
 * Reserved or Intel Reserved and should not be touched.
 */
#define APBASE	0x34 /* Aperture Base Address (0x00000008) */
#define PACCFG	0x50 /* 440LX PAC Configuration Register (0s00_s000_0000_0s00b) */
#define DBC	0x53 /* DRAM Row Type Register (0x83) */
#define DRT	0x55 /* DRAM Row Type Register (0x0000) */
#define DRAMC	0x57 /* DRAM Control (0x01) */
#define DRAMT   0x58 /* DRAM Timing (0x00) */
#define PAM     0x59 /* Programmable Attribute Map, 7 registers (0x00). */
#define DRB     0x60 /* DRAM Row Boundary, 8 registers (0x01). */
#define FDHC    0x68 /* Fixed SDRAM Hole Control (0x00). */
#define DRAMXC	0x6A /* Dram Extended Control Register (0x0000) */
#define MBSC	0x6C /* Memory Buffer Strength Control: (0x55555555) */

#define SMRAM   0x72 /* System Management RAM Control (0x02). */
#define ERRCMD  0x90 /* Error Command Register (0x80). */
#define ERRSTS0 0x91 /* Error Status (0x0000). */
#define ERRSTS1 0x92 /* Error Status (0x0000). */
// TODO: AGP stuff.

/* For convenience: */
#define DRB0    0x60
#define DRB1    0x61
#define DRB2    0x62
#define DRB3    0x63
#define DRB4    0x64
#define DRB5    0x65
#define DRB6    0x66
#define DRB7    0x67

#define PAM0    0x59
#define PAM1    0x5a
#define PAM2    0x5b
#define PAM3    0x5c
#define PAM4    0x5d
#define PAM5    0x5e
#define PAM6    0x5f

