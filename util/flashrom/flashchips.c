/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2000 Silicon Integrated System Corporation
 * Copyright (C) 2004 Tyan Corp
 * Copyright (C) 2005-2007 coresystems GmbH <stepan@openbios.org>
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

#include "flash.h"
#ifndef DISABLE_DOC
#include "msys_doc.h"
#endif

struct flashchip flashchips[] = {
	{"Am29F040B",	AMD_ID, 	AM_29F040B,	512, 64 * 1024,
	 probe_29f040b, erase_29f040b,	write_29f040b},
	{"Am29LV040B",	AMD_ID, 	AM_29LV040B,	512, 64 * 1024,
	 probe_29f040b, erase_29f040b,	write_29f040b},
	{"Am29F016D",	AMD_ID, 	AM_29F016D,	2048, 64 * 1024,
	 probe_29f040b, erase_29f040b,	write_29f040b},
	{"AE49F2008",	ASD_ID,	        ASD_AE49F2008,	256, 128,
	 probe_jedec,	erase_chip_jedec, write_jedec},
	{"At29C040A",	ATMEL_ID,	AT_29C040A,	512, 256,
	 probe_jedec,	erase_chip_jedec, write_jedec},
	{"At29C020",	ATMEL_ID,	AT_29C020,	256, 256,
	 probe_jedec,	erase_chip_jedec, write_jedec},
	{"MX29F002",	MX_ID,		MX_29F002,	256, 64 * 1024,
	 probe_29f002,	erase_29f002, 	write_29f002},
	{"MX25L4005",	MX_ID,		MX_25L4005,	512, 4 * 1024,
	 probe_spi,	generic_spi_chip_erase,	generic_spi_chip_write},
	{"SST29EE020A", SST_ID,		SST_29EE020A,	256, 128,
	 probe_jedec,	erase_chip_jedec, write_jedec},
	{"SST28SF040A", SST_ID,		SST_28SF040,	512, 256,
	 probe_28sf040, erase_28sf040, write_28sf040},
	{"SST39SF010A", SST_ID,		SST_39SF010,	128, 4096,
	 probe_jedec,	erase_chip_jedec, write_39sf020},
	{"SST39SF020A", SST_ID,		SST_39SF020,	256, 4096,
	 probe_jedec,	erase_chip_jedec, write_39sf020},
	{"SST39SF040",  SST_ID,		SST_39SF040,	512, 4096,
	 probe_jedec,	erase_chip_jedec, write_39sf020},
	{"SST39VF020",	SST_ID,		SST_39VF020,	256, 4096,
	 probe_jedec,	erase_chip_jedec, write_39sf020},
// assume similar to 004B, ignoring data sheet
	{"SST49LF040B",	SST_ID,		SST_49LF040B, 	512, 64 * 1024,
	 probe_sst_fwhub, erase_sst_fwhub, write_sst_fwhub},

	{"SST49LF040",	SST_ID,		SST_49LF040, 	512, 4096,
	 probe_jedec, 	erase_49lf040, write_49lf040},
	{"SST49LF020A",	SST_ID,		SST_49LF020A, 	256, 16 * 1024,
	 probe_jedec, 	erase_49lf040, write_49lf040},
	{"SST49LF080A",	SST_ID,		SST_49LF080A,	1024, 4096,
	 probe_jedec,	erase_49lf040, write_49lf040},
	{"SST49LF002A/B", SST_ID,	SST_49LF002A,	256, 16 * 1024,
	 probe_sst_fwhub, erase_sst_fwhub, write_sst_fwhub},
	{"SST49LF003A/B", SST_ID,	SST_49LF003A,	384, 64 * 1024,
	 probe_sst_fwhub, erase_sst_fwhub, write_sst_fwhub},
	{"SST49LF004A/B", SST_ID,	SST_49LF004A,	512, 64 * 1024,
	 probe_sst_fwhub, erase_sst_fwhub, write_sst_fwhub},
	{"SST49LF008A", SST_ID,		SST_49LF008A, 	1024, 64 * 1024 ,
	 probe_sst_fwhub, erase_sst_fwhub, write_sst_fwhub},
	{"SST49LF004C", SST_ID,		SST_49LF004C,	512, 4 * 1024,
	 probe_49lfxxxc, erase_49lfxxxc, write_49lfxxxc},
	{"SST49LF008C", SST_ID,		SST_49LF008C, 	1024, 4 * 1024 ,
	 probe_49lfxxxc, erase_49lfxxxc, write_49lfxxxc},
	{"SST49LF016C", SST_ID,		SST_49LF016C, 	2048, 4 * 1024 ,
	 probe_49lfxxxc, erase_49lfxxxc, write_49lfxxxc},
	{"SST49LF160C", SST_ID,		SST_49LF160C, 	2048, 4 * 1024 ,
	 probe_49lfxxxc, erase_49lfxxxc, write_49lfxxxc},
	{"Pm49FL002",	PMC_ID,		PMC_49FL002,	256, 16 * 1024,
	 probe_jedec,	erase_chip_jedec, write_49fl004},
	{"Pm49FL004",	PMC_ID,		PMC_49FL004,	512, 64 * 1024,
	 probe_jedec,	erase_chip_jedec, write_49fl004},
	{"W29C011",	WINBOND_ID,	W_29C011,	128, 128,
	 probe_jedec,	erase_chip_jedec, write_jedec},
	{"W29C040P",	WINBOND_ID,	W_29C040P,	512, 256,
	 probe_jedec,	erase_chip_jedec, write_jedec},
	{"W29C020C", 	WINBOND_ID, 	W_29C020C,	256, 128,
	 probe_jedec, 	erase_chip_jedec, write_jedec},
	{"W29EE011",	WINBOND_ID,	W_29C011,	128, 128,
	 probe_w29ee011,erase_chip_jedec, write_jedec},
	{"W49F002U", 	WINBOND_ID, 	W_49F002U,	256, 128,
	 probe_jedec,	erase_chip_jedec, write_49f002},
	{"W49V002A", 	WINBOND_ID, 	W_49V002A,	256, 128,
	 probe_jedec,	erase_chip_jedec, write_49f002},
	{"W49V002FA", 	WINBOND_ID, 	W_49V002FA,	256, 128,
	 probe_jedec,	erase_chip_jedec, write_49f002},
	{"W39V040FA", 	WINBOND_ID, 	W_39V040FA,	512, 64*1024,
	 probe_jedec,	erase_chip_jedec, write_39sf020},
	{"W39V040A", 	WINBOND_ID, 	W_39V040A,	512, 64*1024,
	 probe_jedec,	erase_chip_jedec, write_39sf020},
	{"W39V040B",    WINBOND_ID,     W_39V040B,      512, 64*1024,
	 probe_jedec,   erase_chip_jedec, write_39sf020},
	{"W39V080A", 	WINBOND_ID, 	W_39V080A,	1024, 64*1024,
	 probe_jedec,	erase_chip_jedec, write_39sf020},
	{"M29F002B",	ST_ID, 		ST_M29F002B,	256, 64 * 1024,
	 probe_jedec,	erase_chip_jedec, write_jedec},
	{"M50FW040",	ST_ID, 		ST_M50FW040,	512, 64 * 1024,
	 probe_jedec,	erase_chip_jedec, write_jedec},
	{"M29W040B",	ST_ID, 		ST_M29W040B,	512, 64 * 1024,
	 probe_jedec,	erase_chip_jedec, write_jedec},
	{"M29F002T/NT",	ST_ID, 		ST_M29F002T,	256, 64 * 1024,
	 probe_jedec,	erase_chip_jedec, write_jedec},
	{"M29F400BT",	ST_ID,		ST_M29F400BT,	512, 64 * 1024,
	 probe_m29f400bt, erase_m29f400bt, write_linuxbios_m29f400bt},
	{"M50FLW040A",	ST_ID,		ST_M50FLW040A,	512,	64 * 1024,
	 probe_jedec,	erase_chip_jedec,	write_jedec},
	{"M50FLW040B",	ST_ID,		ST_M50FLW040B,	512,	64 * 1024,
	 probe_jedec,	erase_chip_jedec,	write_jedec},
	{"M50FLW080A",	ST_ID,		ST_M50FLW080A,	1024,	64 * 1024,
	 probe_jedec,	erase_chip_jedec,	write_jedec},
	{"M50FLW080B",	ST_ID,		ST_M50FLW080B,	1024,	64 * 1024,
	 probe_jedec,	erase_chip_jedec,	write_jedec},
	{"M50FW080",	ST_ID,		ST_M50FW080,	1024,	64 * 1024,
	 probe_jedec,	erase_chip_jedec,	write_jedec},
	{"M50FW016",	ST_ID,		ST_M50FW016,	2048,	64 * 1024,
	 probe_jedec,	erase_chip_jedec,	write_jedec},
	{"M50LPW116",	ST_ID,		ST_M50LPW116,	2048,	64 * 1024,
	 probe_jedec,	erase_chip_jedec,	write_jedec},
	{"M29W010B",	ST_ID,		ST_M29W010B,	128,	16 * 1024,
	 probe_jedec,	erase_chip_jedec,	write_jedec},
	{"M29F040B",	ST_ID, 		ST_M29F040B,	512, 64 * 1024,
	 probe_29f040b, erase_29f040b,	write_29f040b},
	{"82802ab",	137,		173,		512, 64 * 1024,
	 probe_82802ab, erase_82802ab,	write_82802ab},
	{"82802ac",	137,		172,		1024, 64 * 1024,
	 probe_82802ab, erase_82802ab,	write_82802ab},
	{"F49B002UA",   EMST_ID,        EMST_F49B002UA, 256, 4096,
         probe_jedec,   erase_chip_jedec, write_49f002},
#ifndef DISABLE_DOC
	{"MD-2802 (M-Systems DiskOnChip Millennium Module)",
	 		MSYSTEMS_ID,	MSYSTEMS_MD2802,8, 8 * 1024,
	 probe_md2802, erase_md2802, write_md2802, read_md2802},
#endif
	{"LHF00L04",	SHARP_ID,	SHARP_LHF00L04,	1024, 64 * 1024,
	 probe_lhf00l04, erase_lhf00l04,  write_lhf00l04},
	{"S29C51001T", 	SYNCMOS_ID, 	S29C51001T,	128, 128,
	 probe_jedec,	erase_chip_jedec, write_49f002},
	{"S29C51002T", 	SYNCMOS_ID, 	S29C51002T,	256, 128,
	 probe_jedec,	erase_chip_jedec, write_49f002},
	{"S29C51004T", 	SYNCMOS_ID, 	S29C51004T,	512, 128,
	 probe_jedec,	erase_chip_jedec, write_49f002},
	{"S29C31004T", 	SYNCMOS_ID, 	S29C31004T,	512, 128,
	 probe_jedec,	erase_chip_jedec, write_49f002},
	{NULL,}
};
