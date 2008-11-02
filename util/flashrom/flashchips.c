/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2000 Silicon Integrated System Corporation
 * Copyright (C) 2004 Tyan Corp
 * Copyright (C) 2005-2008 coresystems GmbH <stepan@openbios.org>
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

/**
 * List of supported flash ROM chips.
 *
 * Please keep the list sorted by vendor name and chip name, so that
 * the output of 'flashrom -L' is alphabetically sorted.
 */
struct flashchip flashchips[] = {
	/**********************************************************************************************************************************************************************************************************************/
	/* Vendor	Chip			Vendor ID	Chip ID			TODO	TODO		Test status	Probe function		Erase function			Write function		Read function */
	/**********************************************************************************************************************************************************************************************************************/
	{"AMD",		"Am29F002(N)BB",	AMD_ID,		AM_29F002BB,		256,	256,		TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_en29f002a},
	{"AMD",		"Am29F002(N)BT",	AMD_ID,		AM_29F002BT,		256,	256,		TEST_OK_PREW,	probe_jedec,		erase_chip_jedec,		write_en29f002a},
	{"AMD",		"Am29F016D",		AMD_ID,		AM_29F016D,		2048,	64 * 1024,	TEST_UNTESTED,	probe_29f040b,		erase_29f040b,			write_29f040b},
	{"AMD",		"Am29F040B",		AMD_ID,		AM_29F040B,		512,	64 * 1024,	TEST_OK_PREW,	probe_29f040b,		erase_29f040b,			write_29f040b},
	{"AMD",		"Am29LV040B",		AMD_ID,		AM_29LV040B,		512,	64 * 1024,	TEST_UNTESTED,	probe_29f040b,		erase_29f040b,			write_29f040b},
	{"ASD",		"AE49F2008",		ASD_ID,		ASD_AE49F2008,		256,	128,		TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_jedec},
	{"Atmel",	"AT29C020",		ATMEL_ID,	AT_29C020,		256,	256,		TEST_OK_PREW,	probe_jedec,		erase_chip_jedec,		write_jedec},
	{"Atmel",	"AT29C040A",		ATMEL_ID,	AT_29C040A,		512,	256,		TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_jedec},
	{"Atmel",	"AT49F002(N)",		ATMEL_ID,	AT_49F002N,		256,	256,		TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_49f002},
	{"Atmel",	"AT49F002(N)T",		ATMEL_ID,	AT_49F002NT,		256,	256,		TEST_OK_PREW,	probe_jedec,		erase_chip_jedec,		write_49f002},
	{"Atmel",	"AT25DF321",		ATMEL_ID,	AT_25DF321,		4096,	256,		TEST_OK_PREW,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write,	spi_chip_read},
	{"AMIC",	"A25L40P",		AMIC_ID,	AMIC_A25L40P,		512,	256,		TEST_OK_PREW,	probe_spi_rdid4,	spi_chip_erase_c7,	spi_chip_write, spi_chip_read},
	{"AMIC",	"A29002B",		AMIC_ID_NOPREFIX, AMIC_A29002B,		256,	64 * 1024,	TEST_UNTESTED,	probe_29f002,		erase_29f002,			write_29f002},
	{"AMIC",	"A29002T",		AMIC_ID_NOPREFIX, AMIC_A29002T,		256,	64 * 1024,	TEST_OK_PREW,	probe_29f002,		erase_29f002,			write_29f002},
	{"AMIC",	"A29040B",		AMIC_ID_NOPREFIX, AMIC_A29040B,		512,	64 * 1024,	TEST_OK_PR,	probe_29f040b,		erase_29f040b,			write_29f040b},
	{"AMIC",	"A49LF040A",		AMIC_ID_NOPREFIX, AMIC_A49LF040A,	512,	64 * 1024,	TEST_OK_PREW,	probe_49fl00x,		erase_49fl00x,			write_49fl00x},
	{"EMST",	"F49B002UA",		EMST_ID,	EMST_F49B002UA,		256,	4096,		TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_49f002},
	{"EON",		"EN29F002(A)(N)B",	EON_ID,		EN_29F002B,		256,	256,		TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_en29f002a},
	{"EON",		"EN29F002(A)(N)T",	EON_ID,		EN_29F002T,		256,	256,		TEST_OK_PREW,	probe_jedec,		erase_chip_jedec,		write_en29f002a},
	{"Fujitsu",	"MBM29F400TC",		FUJITSU_ID,	MBM29F400TC_STRANGE,	512,	64 * 1024,	TEST_UNTESTED,	probe_m29f400bt,	erase_m29f400bt,		write_coreboot_m29f400bt},
	{"Intel",	"82802AB",		INTEL_ID,	173,			512,	64 * 1024,	TEST_OK_PREW,	probe_82802ab,		erase_82802ab,			write_82802ab},
	{"Intel",	"82802AC",		INTEL_ID,	172,			1024,	64 * 1024,	TEST_OK_PREW,	probe_82802ab,		erase_82802ab,			write_82802ab},
	{"Macronix",	"MX25L4005",		MX_ID,		MX_25L4005,		512,	256,		TEST_OK_PREW,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write,	spi_chip_read},
	{"Macronix",	"MX25L8005",		MX_ID,		MX_25L8005,		1024,	256,		TEST_OK_PREW,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write,	spi_chip_read},
	{"Macronix",	"MX25L1605",		MX_ID,		MX_25L1605,		2048,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write,	spi_chip_read},
	{"Macronix",	"MX25L3205",		MX_ID,		MX_25L3205,		4096,	256,		TEST_OK_PREW,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write,	spi_chip_read},
	{"Macronix",	"MX29F002",		MX_ID,		MX_29F002,		256,	64 * 1024,	TEST_UNTESTED,	probe_29f002,		erase_29f002,			write_29f002},
	{"Numonyx",	"M25PE10",		ST_ID,		0x8011,			128,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_d8,	spi_chip_write, spi_chip_read},
	{"Numonyx",	"M25PE20",		ST_ID,		0x8012,			256,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_d8,	spi_chip_write, spi_chip_read},
	{"Numonyx",	"M25PE40",		ST_ID,		0x8013,			256,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_d8,	spi_chip_write, spi_chip_read},
	{"Numonyx",	"M25PE80",		ST_ID,		0x8014,			1024,	256,		TEST_OK_PREW,	probe_spi_rdid,		spi_chip_erase_d8,	spi_chip_write, spi_chip_read},
	{"Numonyx",	"M25PE16",		ST_ID,		0x8015,			2048,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_d8,	spi_chip_write, spi_chip_read},
	{"PMC",		"Pm25LV010",		PMC_ID,		PMC_25LV010,		128,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write,	spi_chip_read},
	{"PMC",		"Pm25LV016B",		PMC_ID,		PMC_25LV016B,		2048,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write,	spi_chip_read},
	{"PMC",		"Pm25LV020",		PMC_ID,		PMC_25LV020,		256,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write,	spi_chip_read},
	{"PMC",		"Pm25LV040",		PMC_ID,		PMC_25LV040,		512,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write,	spi_chip_read},
	{"PMC",		"Pm25LV080B",		PMC_ID,		PMC_25LV080B,		1024,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write,	spi_chip_read},
	{"PMC",		"Pm25LV512",		PMC_ID,		PMC_25LV512,		64,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write,	spi_chip_read},
	{"PMC",		"Pm49FL002",		PMC_ID_NOPREFIX,PMC_49FL002,		256,	16 * 1024,	TEST_OK_PREW,	probe_49fl00x,		erase_49fl00x,		write_49fl00x},
	{"PMC",		"Pm49FL004",		PMC_ID_NOPREFIX,PMC_49FL004,		512,	64 * 1024,	TEST_OK_PREW,	probe_49fl00x,		erase_49fl00x,		write_49fl00x},
	{"Sharp",	"LHF00L04",		SHARP_ID,	SHARP_LHF00L04,		1024,	64 * 1024,	TEST_UNTESTED,	probe_lhf00l04,		erase_lhf00l04,			write_lhf00l04},
	{"Spansion",	"S25FL016A",		SPANSION_ID,	SPANSION_S25FL016A,	2048,	256,		TEST_OK_PREW,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write,	spi_chip_read},
	{"SST",		"SST25VF016B",		SST_ID,		SST_25VF016B,		2048,	256,		TEST_OK_PREW,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write,	spi_chip_read},
	{"SST",		"SST25VF040B",		SST_ID,		SST_25VF040B,		512,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write,	spi_chip_read},
	{"SST",		"SST28SF040A",		SST_ID,		SST_28SF040,		512,	256,		TEST_UNTESTED,	probe_28sf040,		erase_28sf040,			write_28sf040},
	{"SST",		"SST29EE010",		SST_ID,		SST_29EE010,		128,	128,		TEST_OK_PREW,	probe_jedec,		erase_chip_jedec,		write_jedec},
	{"SST",		"SST29LE010",		SST_ID,		SST_29LE010,		128,	128,		TEST_UNTESTED,	probe_jedec,		erase_chip_jedec, 		write_jedec},
	{"SST",		"SST29EE020A",		SST_ID,		SST_29EE020A,		256,	128,		TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_jedec},
	{"SST",		"SST29LE020",		SST_ID,		SST_29LE020,		256,	128,		TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_jedec},
	{"SST",		"SST39SF010A",		SST_ID,		SST_39SF010,		128,	4096,		TEST_OK_PREW,	probe_jedec,		erase_chip_jedec,		write_39sf020},
	{"SST",		"SST39SF020A",		SST_ID,		SST_39SF020,		256,	4096,		TEST_OK_PREW,	probe_jedec,		erase_chip_jedec,		write_39sf020},
	{"SST",		"SST39SF040",		SST_ID,		SST_39SF040,		512,	4096,		TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_39sf020},
	{"SST",		"SST39VF512",		SST_ID,		SST_39VF512,		64,	4096,		TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_39sf020},
	{"SST",		"SST39VF010",		SST_ID,		SST_39VF010,		128,	4096,		TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_39sf020},
	{"SST",		"SST39VF020",		SST_ID,		SST_39VF020,		256,	4096,		TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_39sf020},
	{"SST",		"SST39VF040",		SST_ID,		SST_39VF040,		512,	4096,		TEST_OK_PROBE,	probe_jedec,		erase_chip_jedec,		write_39sf020},
// assume similar to 004B, ignoring data sheet
	{"SST",		"SST49LF002A/B",	SST_ID,		SST_49LF002A,		256,	16 * 1024,	TEST_OK_PREW,	probe_sst_fwhub,	erase_sst_fwhub,		write_sst_fwhub},
	{"SST",		"SST49LF003A/B",	SST_ID,		SST_49LF003A,		384,	64 * 1024,	TEST_UNTESTED,	probe_sst_fwhub,	erase_sst_fwhub,		write_sst_fwhub},
	{"SST",		"SST49LF004A/B",	SST_ID,		SST_49LF004A,		512,	64 * 1024,	TEST_OK_PREW,	probe_sst_fwhub,	erase_sst_fwhub,		write_sst_fwhub},
	{"SST",		"SST49LF004C",		SST_ID,		SST_49LF004C,		512,	4 * 1024,	TEST_UNTESTED,	probe_49lfxxxc,		erase_49lfxxxc,			write_49lfxxxc},
	{"SST",		"SST49LF008A",		SST_ID,		SST_49LF008A,		1024,	64 * 1024,	TEST_OK_PREW,	probe_sst_fwhub,	erase_sst_fwhub,		write_sst_fwhub},
	{"SST",		"SST49LF008C",		SST_ID,		SST_49LF008C,		1024,	4 * 1024,	TEST_UNTESTED,	probe_49lfxxxc,		erase_49lfxxxc,			write_49lfxxxc},
	{"SST",		"SST49LF016C",		SST_ID,		SST_49LF016C,		2048,	4 * 1024,	TEST_OK_PREW,	probe_49lfxxxc,		erase_49lfxxxc,			write_49lfxxxc},
	{"SST",		"SST49LF020A",		SST_ID,		SST_49LF020A,		256,	16 * 1024,	TEST_UNTESTED,	probe_jedec,		erase_49lf040,			write_49lf040},
	{"SST",		"SST49LF040",		SST_ID,		SST_49LF040,		512,	4096,		TEST_OK_PREW,	probe_jedec,		erase_49lf040,			write_49lf040},
	{"SST",		"SST49LF040B",		SST_ID,		SST_49LF040B,		512,	64 * 1024,	TEST_OK_PREW,	probe_sst_fwhub,	erase_sst_fwhub,		write_sst_fwhub},
	{"SST",		"SST49LF080A",		SST_ID,		SST_49LF080A,		1024,	4096,		TEST_OK_PREW,	probe_jedec,		erase_49lf040,			write_49lf040},
	{"SST",		"SST49LF160C",		SST_ID,		SST_49LF160C,		2048,	4 * 1024,	TEST_OK_PREW,	probe_49lfxxxc,		erase_49lfxxxc,			write_49lfxxxc},
	{"ST",		"M25P05-A",		ST_ID,		ST_M25P05A,		64,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write, spi_chip_read},
	{"ST",		"M25P10-A",		ST_ID,		ST_M25P10A,		128,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write, spi_chip_read},
	{"ST",		"M25P20",		ST_ID,		ST_M25P20,		256,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write, spi_chip_read},
	{"ST",		"M25P40",		ST_ID,		ST_M25P40,		512,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write, spi_chip_read},
	{"ST",		"M25P40-old",		ST_ID,		ST_M25P40_RES,		512,	256,		TEST_UNTESTED,	probe_spi_res,		spi_chip_erase_c7,	spi_chip_write, spi_chip_read},
	{"ST",		"M25P80",		ST_ID,		ST_M25P80,		1024,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write, spi_chip_read},
	{"ST",		"M25P16",		ST_ID,		ST_M25P16,		2048,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write, spi_chip_read},
	{"ST",		"M25P32",		ST_ID,		ST_M25P32,		4096,	256,		TEST_OK_PREW,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write, spi_chip_read},
	{"ST",		"M25P64",		ST_ID,		ST_M25P64,		8192,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write, spi_chip_read},
	{"ST",		"M25P128",		ST_ID,		ST_M25P128,		16384,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write, spi_chip_read},
	{"ST",		"M29F002B",		ST_ID,		ST_M29F002B,		256,	64 * 1024,	TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_jedec},
	{"ST",		"M29F002T/NT",		ST_ID,		ST_M29F002T,		256,	64 * 1024,	TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_jedec},
	{"ST",		"M29F040B",		ST_ID,		ST_M29F040B,		512,	64 * 1024,	TEST_OK_PREW,	probe_29f040b,		erase_29f040b,			write_29f040b},
	{"ST",		"M29F400BT",		ST_ID,		ST_M29F400BT,		512,	64 * 1024,	TEST_UNTESTED,	probe_m29f400bt,	erase_m29f400bt,		write_coreboot_m29f400bt},
	{"ST",		"M29W010B",		ST_ID,		ST_M29W010B,		128,	16 * 1024,	TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_jedec},
	{"ST",		"M29W040B",		ST_ID,		ST_M29W040B,		512,	64 * 1024,	TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_jedec},
	{"ST",		"M50FLW040A",		ST_ID,		ST_M50FLW040A,		512,	64 * 1024,	TEST_UNTESTED,	probe_stm50flw0x0x,	erase_stm50flw0x0x,		write_stm50flw0x0x},
	{"ST",		"M50FLW040B",		ST_ID,		ST_M50FLW040B,		512,	64 * 1024,	TEST_UNTESTED,	probe_stm50flw0x0x,	erase_stm50flw0x0x,		write_stm50flw0x0x},
	{"ST",		"M50FLW080A",		ST_ID,		ST_M50FLW080A,		1024,	64 * 1024,	TEST_OK_PREW,	probe_stm50flw0x0x,	erase_stm50flw0x0x,		write_stm50flw0x0x},
	{"ST",		"M50FLW080B",		ST_ID,		ST_M50FLW080B,		1024,	64 * 1024,	TEST_UNTESTED,	probe_stm50flw0x0x,	erase_stm50flw0x0x,		write_stm50flw0x0x},
	{"ST",		"M50FW002",		ST_ID,		ST_M50FW002,		256,	64 * 1024,	TEST_UNTESTED,	probe_49lfxxxc,		NULL,			NULL},
	{"ST",		"M50FW016",		ST_ID,		ST_M50FW016,		2048,	64 * 1024,	TEST_UNTESTED,	probe_82802ab,		erase_82802ab,			write_82802ab},
	{"ST",		"M50FW040",		ST_ID,		ST_M50FW040,		512,	64 * 1024,	TEST_OK_PREW,	probe_82802ab,		erase_82802ab,			write_82802ab},
	{"ST",		"M50FW080",		ST_ID,		ST_M50FW080,		1024,	64 * 1024,	TEST_UNTESTED,	probe_82802ab,		erase_82802ab,			write_82802ab},
	{"ST",		"M50LPW116",		ST_ID,		ST_M50LPW116,		2048,	64 * 1024,	TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_jedec},
	{"SyncMOS",	"S29C31004T",		SYNCMOS_ID,	S29C31004T,		512,	128,		TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_49f002},
	{"SyncMOS",	"S29C51001T",		SYNCMOS_ID,	S29C51001T,		128,	128,		TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_49f002},
	{"SyncMOS",	"S29C51002T",		SYNCMOS_ID,	S29C51002T,		256,	128,		TEST_OK_PREW,	probe_jedec,		erase_chip_jedec,		write_49f002},
	{"SyncMOS",	"S29C51004T",		SYNCMOS_ID,	S29C51004T,		512,	128,		TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_49f002},
	{"Winbond",	"W25x10",		WINBOND_NEX_ID,	W_25X10,		128,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write, spi_chip_read},
	{"Winbond",	"W25x20",		WINBOND_NEX_ID,	W_25X20,		256,	256,		TEST_UNTESTED,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write, spi_chip_read},
	{"Winbond",	"W25x40",		WINBOND_NEX_ID,	W_25X40,		512,	256,		TEST_OK_PREW,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write, spi_chip_read},
	{"Winbond",	"W25x80",		WINBOND_NEX_ID,	W_25X80,		1024,	256,		TEST_OK_PREW,	probe_spi_rdid,		spi_chip_erase_c7,	spi_chip_write, spi_chip_read},
	{"Winbond",	"W29C011",		WINBOND_ID,	W_29C011,		128,	128,		TEST_OK_PREW,	probe_jedec,		erase_chip_jedec,		write_jedec},
	{"Winbond",	"W29C020C",		WINBOND_ID,	W_29C020C,		256,	128,		TEST_OK_PREW,	probe_jedec,		erase_chip_jedec,		write_jedec},
	{"Winbond",	"W29C040P",		WINBOND_ID,	W_29C040P,		512,	256,		TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_jedec},
	{"Winbond",	"W29EE011",		WINBOND_ID,	W_29C011,		128,	128,		TEST_OK_PREW,	probe_w29ee011,		erase_chip_jedec,		write_jedec},
	{"Winbond",	"W39V040A",		WINBOND_ID,	W_39V040A,		512,	64*1024,	TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_39sf020},
	{"Winbond",	"W39V040B",		WINBOND_ID,	W_39V040B,		512,	64*1024,	TEST_OK_PREW,	probe_jedec,		erase_chip_jedec,		write_39sf020},
	{"Winbond",	"W39V040C",		0xda,		0x50,			512,	64*1024,	TEST_OK_PREW,	probe_w39v040c,		erase_w39v040c,			write_w39v040c},
	{"Winbond",	"W39V040FA",		WINBOND_ID,	W_39V040FA,		512,	64*1024,	TEST_OK_PREW,	probe_jedec,		erase_chip_jedec,		write_39sf020},
	{"Winbond",	"W39V080A",		WINBOND_ID,	W_39V080A,		1024,	64*1024,	TEST_OK_PREW,	probe_jedec,		erase_chip_jedec,		write_39sf020},
	{"Winbond",	"W49F002U",		WINBOND_ID,	W_49F002U,		256,	128,		TEST_OK_PREW,	probe_jedec,		erase_chip_jedec,		write_49f002},
	{"Winbond",	"W49V002A",		WINBOND_ID,	W_49V002A,		256,	128,		TEST_OK_PREW,	probe_jedec,		erase_chip_jedec,		write_49f002},
	{"Winbond",	"W49V002FA",		WINBOND_ID,	W_49V002FA,		256,	128,		TEST_UNTESTED,	probe_jedec,		erase_chip_jedec,		write_49f002},
	{"Winbond",	"W39V080FA",		WINBOND_ID,	W_39V080FA,		1024,	64*1024,	TEST_OK_PREW,	probe_winbond_fwhub,	erase_winbond_fwhub,		write_winbond_fwhub},
	{"Winbond",	"W39V080FA (dual mode)",WINBOND_ID,	W_39V080FA_DM,		512,	64*1024,	TEST_UNTESTED,	probe_winbond_fwhub,	erase_winbond_fwhub,		write_winbond_fwhub},

	{"EON",		"unknown EON SPI chip",	EON_ID_NOPREFIX,GENERIC_DEVICE_ID,	0,	256,		TEST_UNTESTED,	probe_spi_rdid,		NULL,				NULL},
	{"Macronix",	"unknown Macronix SPI chip",	MX_ID,	GENERIC_DEVICE_ID,	0,	256,		TEST_UNTESTED,	probe_spi_rdid,		NULL,				NULL},
	{"PMC",		"unknown PMC SPI chip",	PMC_ID,		GENERIC_DEVICE_ID,	0,	256,		TEST_UNTESTED,	probe_spi_rdid,		NULL,				NULL},
	{"SST",		"unknown SST SPI chip",	SST_ID,		GENERIC_DEVICE_ID,	0,	256,		TEST_UNTESTED,	probe_spi_rdid,		NULL,				NULL},
	{"ST",		"unknown ST SPI chip",	ST_ID,		GENERIC_DEVICE_ID,	0,	256,		TEST_UNTESTED,	probe_spi_rdid,		NULL,				NULL},

	{NULL,}
};
