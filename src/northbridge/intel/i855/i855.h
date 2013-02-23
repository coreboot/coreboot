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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* Host-Hub Interface Bridge */
#define GMC      0x50 /* GMCH Misc. Control (0x0000) */
#define GGC      0x52 /* GMCH Graphics Control (0x0030) */
#define DAFC     0x54 /* Device and Function Control (0x0000) */
#define FDHC     0x58 /* Fixed Dram Hole Control */
#define PAM0     0x59 /* Programmable Attribute Map #0 (0x00) */
#define PAM1     0x5a /* Programmable Attribute Map #1 (0x00) */
#define PAM2     0x5b /* Programmable Attribute Map #2 (0x00) */
#define PAM3     0x5c /* Programmable Attribute Map #3 (0x00) */
#define PAM4     0x5d /* Programmable Attribute Map #4 (0x00) */
#define PAM5     0x5e /* Programmable Attribute Map #5 (0x00) */
#define PAM6     0x5f /* Programmable Attribute Map #6 (0x00) */
#define SMRAM    0x60 /* System Management RAM Control (0x02) */
#define ESMRAMC  0x61 /* Extended System Management RAM Control (0x38) */
#define ERRSTS   0x62 /* Error Status (0x0000) */
#define ERRCMD   0x64 /* Error Command (0x0000) */
#define SMICMD   0x66 /* SMI Command (0x00) */
#define SCICMD   0x67 /* SCI Command (0x00) */
#define SHIC     0x74 /* Secondary Host Interface Control Register (0x00006010) */
#define ACAPID   0xA0 /* AGP Capability Identifier (0x00200002) */
#define AGPSTAT  0xA4 /* AGP Status Register (0x1f000217) */
#define AGPCMD   0xA8 /* AGP Command (0x0000) */
#define AGPCTRL  0xB0 /* AGP Control (0x0000) */
#define AFT      0xB2 /* AGP Functional Test (0xe9f0) */
#define ATTBASE  0xB8 /* Aperture Translation Table Base (0x00000000) */
#define AMTT     0xBC /* AGP Interface Multi Transaction Timer (0x00) */
#define LPTT     0xBD /* Low Priority Transaction Timer (0x00) */
#define HEM      0xF0 /* Host Error Control/Status/Obs (0x00000000) */

/* Main Memory Control */
#define DRB      0x40 /* DRAM Row 0-3 Boundary (0x00000000) */
#define DRA      0x50 /* DRAM Row 0-3 Attribute (0x7777) */
#define DRT      0x60 /* DRAM Timing (0x18004425) */
#define PWRMG    0x68 /* DRAM Controller Power Management Control (0x00000000) */
#define DRC      0x70 /* DRAM Controller Mode (0x00000081) */
#define DTC      0xA0 /* DRAM Throttling Control (0x00000000) */

#define DRT_CAS_MASK    (3 << 5)
#define DRT_CAS_2_0     (1 << 5)
#define DRT_CAS_2_5     (0 << 5)

#define DRT_TRP_MASK    3
#define DRT_TRP_4       0
#define DRT_TRP_3       1
#define DRT_TRP_2       2

#define DRT_RCD_MASK    (3 << 2)
#define DRT_RCD_4       (0 << 2)
#define DRT_RCD_3       (1 << 2)
#define DRT_RCD_2       (2 << 2)

#define DRT_TRAS_MIN_MASK    (3 << 9)
#define DRT_TRAS_MIN_8       (0 << 9)
#define DRT_TRAS_MIN_7       (1 << 9)
#define DRT_TRAS_MIN_6       (2 << 9)
#define DRT_TRAS_MIN_5       (3 << 9)
