/* TODO:
 * AGP #defines
 * GPL header
 */

/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Travelping GmbH <info@travelping.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/* size, default value */
/* DRAM Controller (device 0) */
#define APBASE		0x10 /* 32 bit, 0x8 */
// #define AGPM		0x51 /* 8 bit, 0x0 */
// #define ESMRAMC		0x9e /* 8 bit, 0x38 */
// #define ACAPID		0xa0 /* 32bit, 0x300002 (RO) */
// #define AGPSTAT		0xa4 /* 32bit, AGP 2.0: 0x1f004217 (RO) or AGP 3.0: 0x1f004a13 (RO) */
// #define AGPCMD		0xa8 /* 32bit, AGP 2.0: 0x0 or AGP 3.0: 0xa00 */
// #define AGPCTRL		0xb0 /* 32bit, 0x0 */
#define AMTT		0xbc /* 8 bit, 0x10 */
#define APSIZE		0xb4 /* 8 bit, 0x0 */
// #define ATTBASE		0xb8 /* 32 bit, 0x0 */
#define TOUD		0xc4 /* 16 bit, 0x400 */
#define GMCHCFG		0xc6 /* 16 bit, 0x0 */
#define ERRSTS		0xc8 /* 16 bit, 0x0 */
// #define ERRCMD		0xca /* 16 bit, 0x0 */
// #define FDHC		0x97 /* 8 bit, 0x0 */
// #define FPLLCONT	0x60 /* 8 bit, 0x0 */
#define GC			0x52 /* 8 bit (looks wrong), 0x1000 */
#define LPTT		0xbd /* 8 bit, 0x10 */
#define PCISTS		0x10 /* 16 bit, 0x90 */
// #define SMRAM		0x9D /* 8 bit, 0x2 */
#define SVID		0x2c /* 16 bit, 0x0 */
#define SID		0x2e /* 16 bit, 0x0 */
// #define CSABCONT	0x53 /* 8 bit, 0x0 */
#define VID 0x0 /* 16 bit, 0x8086 */
#define DID 0x2 /* 16 bit, 0x2570 */
/* PCI-to-AGP bridge (device 1) */
// #define PCICMD1		0x04 /* 16 bit, 0x0 */
// #define SMLT1		0x0d /* 8 bit, 0x0 */
// #define SBUSN1		0x19 /* 8 bit, 0x0 */
// #define SUBUSN1		0x1a /* 8 bit, 0x0 */
// #define PMBASE1		0x24 /* 16 bit, 0xfff0 */
// #define PMLIMIT1	0x26 /* 16 bit, 0x0 */
// #define BCTRL1		0x3e /* 8 bit, 0x0 */
// #define ERRCMD1		0x40 /* 8 bit, 0x0 */

/* Integrated Graphics Device (device 2) */
// #define PCICMD2		0x04 /* 16 bit, 0x0 */
// #define GMADR		0x10 /* 32 bit, 0x8 */
// #define MMADR		0x14 /* 32 bit, 0x0 */
// #define IOBAR		0x18 /* 32 bit, 0x1 */
// #define SVID2		0x2c /* 16 bit, 0x0 */
// #define SID2		0x2e /* 16 bit, 0x0 */
// #define INTRLINE	0x3c /* 8 bit, 0x0 */
// #define PMCS		0xd4 /* 16 bit, 0x0 */
// #define SWSMI		0xe0 /* 16 bit, 0x0 */

/* PCI-to-CSA bridge (device 3) */
// #define PCICMD3		0x04 /* 16 bit, 0x0 */
// #define IOBASE3		0x1c /* 8 bit, 0xf0 */
// #define IOLIMIT3	0x1d /* 8 bit, 0x0 */
// #define MBASE3		0x20 /* 16 bit, 0xfff0 */
// #define MLIMIT3		0x22 /* 16 bit, 0x0 */
// #define PMBASE3		0x24 /* 16 bit, 0xfff0 */
// #define PMLIMIT3	0x26 /* 16 bit, 0x0 */
// #define BCTRL3		0x3e /* 8 bit, 0x0 */
// #define CSACNTRL	0x50 /* 32 bit, 0xe042802 */

/* Overflow device (device 6) */
#define PCICMD6		0x04 /* 16 bit (looks wrong), 0x0 */
#define BAR6		0x10 /* 32 bit, 0x0 */
// #define SVID6		0x2c /* 16 bit, 0x0 */
// #define SID6		0x2e /* 16 bit, 0x0 */
