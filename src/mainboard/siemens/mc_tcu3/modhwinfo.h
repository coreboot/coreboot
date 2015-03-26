/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Siemens AG
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
 * Foundation, Inc.
 */

#ifndef _MODHWINFO_H_
#define _MODHWINFO_H_

#include <types.h>

#define LEN_MAGIC_NUM		8
#define LEN_MLFB		20
#define LEN_SERIAL_NUM		16
#define LEN_HW_REVISION		2
#define LEN_MAC_ADDRESS		6
#define LEN_SPD			128
#define NUMBER_OF_MAC_ADDR	4
#define LEN_EDID		128
#define LEN_MAIN_HWINFO		509
#define LEN_SHORT_INFO		289
#define LEN_EDID_INFO		181
#define NEXT_OFFSET_HWINFO	476
#define NEXT_OFFSET_SIB		260
#define NEXT_OFFSET_EDID	176
#define HWI_LEN_OFFSET		12

/* Define some masks and values */
#define SIB_HWINIT_IDX		0x0e
#define SIB_COLOR_6BIT		0x00
#define SIB_COLOR_8BIT		0x01
#define SIB_COLOR_10BIT		0x02
#define SIB_DISP_CON_IDX	0x16
#define SIB_LVDS_SINGLE_LANE	0x00
#define SIB_LVDS_DUAL_LANE	0x05

struct PhysDevStruc {
    u8  bustype;
    u8 	devfn;
    u16	bus;
} __attribute__ ((packed));


struct hwinfo {
// Offset 0x0
	char	magicNumber[LEN_MAGIC_NUM];
// Offset 0x08
	u8	versionID[4];
// Offset 0x0c
	u16	length;
// Offset 0x0e
	char	BGR_Name[128];
//Offset 0x8e
	char	MLFB[LEN_MLFB];
// Offset 0xa2
	u8	uniqueNumber[16];
// Offset 0xb2
	u8	fill_1[12];
// Offset 0xbe
	u8	hwRevision[LEN_HW_REVISION];
//Offset 0xc0
	u8	macAddress1[LEN_MAC_ADDRESS];
	u8	numOfAuxMacAddr1;
	u8	fill_2;
//Offset 0xc8
	u8	macAddress2[LEN_MAC_ADDRESS];
	u8	numOfAuxMacAddr2;
	u8	fill_3;
//Offset 0xd0
	u8	macAddress3[LEN_MAC_ADDRESS];
	u8	numOfAuxMacAddr3;
	u8	fill_4;
//Offset 0xd8
	u8	macAddress4[LEN_MAC_ADDRESS];
	u8	numOfAuxMacAddr4;
	u8	fill_5;
// Offset 0xe0
	u8	SPD[LEN_SPD];
// Offset 0x160
	u8	fill_6[88];
// Offset 0x1b8
	u32	featureFlags;
// Offset 0x1bc
	u8	fill_7[4];
// Offset 0x1c0
	u32	biosFlags;
// Offset 0x1c4
	u8	fill_8[8];
//Offset 0x1cc
	struct PhysDevStruc etherDev[NUMBER_OF_MAC_ADDR];
// Offset 0x1dc
	s32	nextInfoOffset;
// Offset 0x1e0
	u8	fill_9[4];
// Offset 0x1e4
	u32	portRTC;
// Offset 0x1e8
	u8	typeRTC;
// Offset 0x1e9
	u8 fill_10[20];
} __attribute__ ((packed));

struct shortinfo {
// Offset 0x0
	char	magicNumber[LEN_MAGIC_NUM];
// Offset 0x08
	u8	versionID[4];
// Offset 0x0c
	u16	length;
// Offset 0x0e
	char	BGR_Name[128];
//Offset 0x8e
	char	MLFB[LEN_MLFB];
// Offset 0xa2
	u8	uniqueNumber[26];
// Offset 0xbc
	u8	fill_1[12];
//Offset 0xc8
	u8	hwRevision[2];
// Offset 0xca
	u8	fill_2[18];
// Offset 0xdc
	u8	panelFeatures[32];
// Offset 0xfc
	u8	fill_3[8];
// Offset 0x104
	s32	nextInfoOffset;
// Offset 0x108
	u8	fill_4[25];
} __attribute__ ((packed));

struct edidinfo {
// Offset 0x0
	char	magicNumber[LEN_MAGIC_NUM];
// Offset 0x08
	u8	versionID[4];
// Offset 0x0c
	u16	length;
// Offset 0x0e
	u8	fill_1[2];
// Offset 0x10
	u8	edid[LEN_EDID];
// Offset 0x90
	u8	fill_2[32];
// Offset 0xb0
	s32	nextInfoOffset;
// Offset 0xb4
	u8	fill_3;
} __attribute__ ((packed));

u8* get_first_linked_block(char *filename, u8 **starting_adr);
struct hwinfo* get_hwinfo(char *filename);
struct shortinfo* get_shortinfo(char *filename);
struct edidinfo* get_edidinfo(char *filename);
enum cb_err mainboard_get_mac_address(u16 bus, u8 devfn, u8 mac[6]);

#endif /* _MODHWINFO_H_ */
