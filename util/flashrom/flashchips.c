/*
 * flashchips.c: flash programming utility - flash devices
 *
 * Copyright 2000 Silicon Integrated System Corporation
 * Copyright 2004 Tyan Corp
 *	yhlu yhlu@tyan.com add exclude start and end option
 * Copyright 2005 coresystems GmbH <stepan@openbios.org>
 * 
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "flash.h"
#include "jedec.h"
#include "m29f400bt.h"
#include "82802ab.h"
#ifndef DISABLE_DOC
#include "msys_doc.h"
#endif
#include "am29f040b.h"
#include "sst28sf040.h"
#include "sst49lfxxxc.h"
#include "w49f002u.h"
#include "sst39sf020.h"
#include "sst49lf040.h"
#include "pm49fl004.h"
#include "mx29f002.h"
#include "sharplhf00l04.h"
#include "sst_fwhub.h"

struct flashchip flashchips[] = {
	{"Am29F040B",	AMD_ID, 	AM_29F040B,	NULL, 512, 64 * 1024,
	 probe_29f040b, erase_29f040b,	write_29f040b,	NULL},
	{"Am29F016D",	AMD_ID, 	AM_29F016D,	NULL, 2048, 64 * 1024,
	 probe_29f040b, erase_29f040b,	write_29f040b,	NULL},
	{"AE49F2008",	ASD_ID,	        ASD_AE49F2008,	NULL, 256, 128,
	 probe_jedec,	erase_chip_jedec, write_jedec,	NULL},
	{"At29C040A",	ATMEL_ID,	AT_29C040A,	NULL, 512, 256,
	 probe_jedec,	erase_chip_jedec, write_jedec,	NULL},
	{"At29C020",	ATMEL_ID,	AT_29C020,	NULL, 256, 256,
	 probe_jedec,	erase_chip_jedec, write_jedec,	NULL},
	{"Mx29f002",	MX_ID,		MX_29F002,	NULL, 256, 64 * 1024,
	 probe_29f002,	erase_29f002, 	write_29f002,	NULL},
	{"SST29EE020A", SST_ID,		SST_29EE020A,	NULL, 256, 128,
	 probe_jedec,	erase_chip_jedec, write_jedec,	NULL},
	{"SST28SF040A", SST_ID,		SST_28SF040,	NULL, 512, 256,
	 probe_28sf040, erase_28sf040, write_28sf040,	NULL},
	{"SST39SF010A", SST_ID,		SST_39SF010,	NULL, 128, 4096,
	 probe_jedec,	erase_chip_jedec, write_39sf020,NULL},
	{"SST39SF020A", SST_ID,		SST_39SF020,	NULL, 256, 4096,
	 probe_jedec,	erase_chip_jedec, write_39sf020,NULL},
	{"SST39SF040",  SST_ID,		SST_39SF040,	NULL, 512, 4096,
	 probe_jedec,	erase_chip_jedec, write_39sf020,NULL},
	{"SST39VF020",	SST_ID,		SST_39VF020,	NULL, 256, 4096,
	 probe_jedec,	erase_chip_jedec, write_39sf020,NULL},
// assume similar to 004B, ignoring data sheet
	{"SST49LF040B",	SST_ID,		SST_49LF040B, 	NULL, 512, 64 * 1024,
	 probe_sst_fwhub, erase_sst_fwhub, write_sst_fwhub,NULL},

	{"SST49LF040",	SST_ID,		SST_49LF040, 	NULL, 512, 4096,
	 probe_jedec, 	erase_49lf040, write_49lf040,NULL},
	{"SST49LF020A",	SST_ID,		SST_49LF020A, 	NULL, 256, 16 * 1024,
	 probe_jedec, 	erase_49lf040, write_49lf040,NULL},
	{"SST49LF080A",	SST_ID,		SST_49LF080A,	NULL, 1024, 4096,
	 probe_jedec,	erase_49lf040, write_49lf040,NULL},
	{"SST49LF002A/B", SST_ID,	SST_49LF002A,	NULL, 256, 16 * 1024,
	 probe_sst_fwhub, erase_sst_fwhub, write_sst_fwhub, NULL},
	{"SST49LF003A/B", SST_ID,	SST_49LF003A,	NULL, 384, 64 * 1024,
	 probe_sst_fwhub, erase_sst_fwhub, write_sst_fwhub,NULL},
	{"SST49LF004A/B", SST_ID,	SST_49LF004A,	NULL, 512, 64 * 1024,
	 probe_sst_fwhub, erase_sst_fwhub, write_sst_fwhub,NULL},
	{"SST49LF008A", SST_ID,		SST_49LF008A, 	NULL, 1024, 64 * 1024 ,
	 probe_sst_fwhub, erase_sst_fwhub, write_sst_fwhub, NULL},
	{"Pm49FL002",	PMC_ID,		PMC_49FL002,	NULL, 256, 16 * 1024,
	 probe_jedec,	erase_chip_jedec, write_49fl004,NULL},
	{"SST49LF004C", SST_ID,	SST_49LF004C,	NULL, 512, 4 * 1024,
	 probe_49lfxxxc, erase_49lfxxxc, write_49lfxxxc,NULL},
	{"SST49LF008C", SST_ID,		SST_49LF008C, 	NULL, 1024, 4 * 1024 ,
	 probe_49lfxxxc, erase_49lfxxxc, write_49lfxxxc, NULL},
	{"SST49LF016C", SST_ID,		SST_49LF016C, 	NULL, 2048, 4 * 1024 ,
	 probe_49lfxxxc, erase_49lfxxxc, write_49lfxxxc, NULL},
	{"SST49LF160C", SST_ID,		SST_49LF160C, 	NULL, 2048, 4 * 1024 ,
	 probe_49lfxxxc, erase_49lfxxxc, write_49lfxxxc, NULL},
	{"Pm49FL004",	PMC_ID,		PMC_49FL004,	NULL, 512, 64 * 1024,
	 probe_jedec,	erase_chip_jedec, write_49fl004,NULL},
	{"W29C011",	WINBOND_ID,	W_29C011,	NULL, 128, 128,
	 probe_jedec,	erase_chip_jedec, write_jedec,	NULL},
	{"W29C020C", 	WINBOND_ID, 	W_29C020C,	NULL, 256, 128,
	 probe_jedec, 	erase_chip_jedec, write_jedec,	NULL},
	{"W49F002U", 	WINBOND_ID, 	W_49F002U,	NULL, 256, 128,
	 probe_jedec,	erase_chip_jedec, write_49f002, NULL},
	{"W49V002A", 	WINBOND_ID, 	W_49V002A,	NULL, 256, 128,
	 probe_jedec,	erase_chip_jedec, write_49f002, NULL},
	{"W49V002FA", 	WINBOND_ID, 	W_49V002FA,	NULL, 256, 128,
	 probe_jedec,	erase_chip_jedec, write_49f002, NULL},
	{"W39V040A", 	WINBOND_ID, 	W_39V040A,	NULL, 512, 64*1024,
	 probe_jedec,	erase_chip_jedec, write_39sf020, NULL},
	{"W39V040B",    WINBOND_ID,     W_39V040B,      NULL, 512, 64*1024,
	 probe_jedec,   erase_chip_jedec, write_39sf020, NULL},
	{"W39V080A", 	WINBOND_ID, 	W_39V080A,	NULL, 1024, 64*1024,
	 probe_jedec,	erase_chip_jedec, write_39sf020, NULL},
	{"M29F002B",	ST_ID, 		ST_M29F002B,	NULL, 256, 64 * 1024,
	 probe_jedec,	erase_chip_jedec, write_jedec,	NULL},
	{"M29F002T/NT",	ST_ID, 		ST_M29F002T,	NULL, 256, 64 * 1024,
	 probe_jedec,	erase_chip_jedec, write_jedec,	NULL},
	{"M29F400BT",	ST_ID,		ST_M29F400BT,	NULL, 512, 64 * 1024,
	 probe_m29f400bt, erase_m29f400bt, write_linuxbios_m29f400bt, NULL},
	{"M29F040B",	ST_ID, 		ST_M29F040B,	NULL, 512, 64 * 1024,
	 probe_29f040b, erase_29f040b,	write_29f040b,	NULL},
	{"82802ab",	137,		173,		NULL, 512, 64 * 1024,
	 probe_82802ab, erase_82802ab,	write_82802ab,	NULL},
	{"82802ac",	137,		172,		NULL, 1024, 64 * 1024,
	 probe_82802ab, erase_82802ab,	write_82802ab,	NULL},
	{"F49B002UA",   EMST_ID,        EMST_F49B002UA, NULL, 256, 4096,
         probe_jedec,   erase_chip_jedec, write_49f002, NULL},
#ifndef DISABLE_DOC
	{"MD-2802 (M-Systems DiskOnChip Millennium Module)",
	 MSYSTEMS_ID, MSYSTEMS_MD2802,
	 NULL, 8, 8 * 1024,
	 probe_md2802, erase_md2802, write_md2802, read_md2802},
#endif
	{"LHF00L04",	SHARP_ID,		SHARP_LHF00L04,		NULL, 1024, 64 * 1024,
	 probe_lhf00l04, erase_lhf00l04,	write_lhf00l04,	NULL},
	{"S29C51001T", 	SYNCMOS_ID, 	S29C51001T,	NULL, 128, 128,
	 probe_jedec,	erase_chip_jedec, write_49f002, NULL},
	{"S29C51002T", 	SYNCMOS_ID, 	S29C51002T,	NULL, 256, 128,
	 probe_jedec,	erase_chip_jedec, write_49f002, NULL},
	{"S29C51004T", 	SYNCMOS_ID, 	S29C51004T,	NULL, 512, 128,
	 probe_jedec,	erase_chip_jedec, write_49f002, NULL},
	{"S29C31004T", 	SYNCMOS_ID, 	S29C31004T,	NULL, 512, 128,
	 probe_jedec,	erase_chip_jedec, write_49f002, NULL},
	{NULL,}
};


