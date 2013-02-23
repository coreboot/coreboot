/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Arastra, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

#ifndef NORTHBRIDGE_INTEL_I3100_EP80579_H
#define NORTHBRIDGE_INTEL_I3100_EP80579_H

#define SMRBASE   0x14
#define MCHCFG0	  0x50
#define FDHC      0x58
#define PAM       0x59
#define DRB       0x60
#define DRT1      0x64
#define DRA       0x70
#define DRT0      0x78
#define DRC       0x7c
#define ECCDIAG   0x84
#define SDRC      0x88
#define CKDIS     0x8c
#define CKEDIS    0x8d
#define DEVPRES   0x9c
#define  DEVPRES_D0F0 (1 << 0)
#define  DEVPRES_D1F0 (1 << 1)
#define  DEVPRES_D2F0 (1 << 2)
#define  DEVPRES_D3F0 (1 << 3)
#define  DEVPRES_D4F0 (1 << 4)
#define  DEVPRES_D10F0 (1 << 5)
#define EXSMRC    0x9d
#define SMRAM     0x9e
#define EXSMRAMC  0x9f
#define DDR2ODTC  0xb0
#define TOLM      0xc4
#define REMAPBASE 0xc6
#define REMAPLIMIT 0xc8
#define REMAPOFFSET 0xca
#define TOM       0xcc
#define HECBASE   0xce
#define DEVPRES1  0xf4

#define DCALCSR   0x040
#define DCALADDR  0x044
#define DCALDATA  0x048
#define MBCSR     0x140
#define MBADDR    0x144
#define MBDATA    0x148
#define DDRIOMC2  0x268

#endif
