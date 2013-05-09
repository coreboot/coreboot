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

#ifndef __I3100_H__
#define __I3100_H__

#define IURBASE	0X14
#define MCHCFG0	0X50
#define MCHSCRB	0X52
#define FDHC	0X58
#define PAM	0X59
#define DRB	0X60
#define DRA	0X70
#define DRT	0X78
#define DRC	0X7C
#define DRM	0X80
#define DRORC	0X82
#define ECCDIAG	0X84
#define SDRC	0X88
#define CKDIS	0X8C
#define CKEDIS	0X8D
#define DDRCSR	0X9A
#define DEVPRES	0X9C
#define  DEVPRES_D0F0 (1 << 0)
#define  DEVPRES_D1F0 (1 << 1)
#define  DEVPRES_D2F0 (1 << 2)
#define  DEVPRES_D3F0 (1 << 3)
#define  DEVPRES_D4F0 (1 << 4)
#define  DEVPRES_D5F0 (1 << 5)
#define  DEVPRES_D6F0 (1 << 6)
#define  DEVPRES_D7F0 (1 << 7)
#define ESMRC	0X9D
#define SMRC	0X9E
#define EXSMRC	0X9F
#define DDR2ODTC 0XB0
#define TOLM	0XC4
#define REMAPBASE 0XC6
#define REMAPLIMIT 0XC8
#define REMAPOFFSET 0XCA
#define TOM	0XCC
#define EXPECBASE 0XCE
#define DEVPRES1 0XF4
#define  DEVPRES1_D0F1 (1 << 5)
#define  DEVPRES1_D8F0 (1 << 1)
#define MSCFG	0XF6

/* DRC */
#define DRC_NOECC_MODE        (0 << 20)
#define DRC_72BIT_ECC         (1 << 20)

#define RCBA 0xF0
#define DEFAULT_RCBA 0xFEA00000

int bios_reset_detected(void);

#endif
