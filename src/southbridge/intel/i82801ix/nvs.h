/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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

typedef struct {
	/* Miscellaneous */
	u16	osys; /* 0x00 - Operating System */
	u8	smif; /* 0x02 - SMI function call ("TRAP") */
	u8	prm0; /* 0x03 - SMI function call parameter */
	u8	prm1; /* 0x04 - SMI function call parameter */
	u8	scif; /* 0x05 - SCI function call (via _L00) */
	u8	prm2; /* 0x06 - SCI function call parameter */
	u8	prm3; /* 0x07 - SCI function call parameter */
	u8	lckf; /* 0x08 - Global Lock function for EC */
	u8	prm4; /* 0x09 - Lock function parameter */
	u8	prm5; /* 0x0a - Lock function parameter */
	u32	p80d; /* 0x0b - Debug port (IO 0x80) value */
	u8	lids; /* 0x0f - LID state (open = 1) */
	u8	pwrs; /* 0x10 - Power state (AC = 1) */
	u8	dbgs; /* 0x11 - Debug state */
	u8	linx; /* 0x12 - Linux OS */
	u8	dckn; /* 0x13 - PCIe docking state */
	/* Thermal policy */
	u8	actt; /* 0x14 - active trip point */
	u8	psvt; /* 0x15 - passive trip point */
	u8	tc1v; /* 0x16 - passive trip point TC1 */
	u8	tc2v; /* 0x17 - passive trip point TC2 */
	u8	tspv; /* 0x18 - passive trip point TSP */
	u8	crtt; /* 0x19 - critical trip point */
	u8	dtse; /* 0x1a - Digital Thermal Sensor enable */
	u8	dts1; /* 0x1b - DT sensor 1 */
	u8	dts2; /* 0x1c - DT sensor 2 */
	u8	rsvd2;
	/* Battery Support */
	u8	bnum; /* 0x1e - number of batteries */
	u8	b0sc, b1sc, b2sc; /* 0x1f-0x21 - stored capacity */
	u8	b0ss, b1ss, b2ss; /* 0x22-0x24 - stored status */
	u8	rsvd3[3];
	/* Processor Identification */
	u8	apic; /* 0x28 - APIC enabled */
	u8	mpen; /* 0x29 - MP capable/enabled */
	u8	pcp0; /* 0x2a - PDC CPU/CORE 0 */
	u8	pcp1; /* 0x2b - PDC CPU/CORE 1 */
	u8	ppcm; /* 0x2c - Max. PPC state */
	u8	rsvd4[5];
	/* Super I/O & CMOS config */
	u8	natp; /* 0x32 - SIO type */
	u8	cmap; /* 0x33 - */
	u8	cmbp; /* 0x34 - */
	u8	lptp; /* 0x35 - LPT port */
	u8	fdcp; /* 0x36 - Floppy Disk Controller */
	u8	rfdv; /* 0x37 - */
	u8	hotk; /* 0x38 - Hot Key */
	u8	rtcf;
	u8	util;
	u8	acin;
	/* Integrated Graphics Device */
	u8	igds; /* 0x3c - IGD state */
	u8	tlst; /* 0x3d - Display Toggle List Pointer */
	u8	cadl; /* 0x3e - currently attached devices */
	u8	padl; /* 0x3f - previously attached devices */
	u16	cste; /* 0x40 - current display state */
	u16	nste; /* 0x42 - next display state */
	u16	sste; /* 0x44 - set display state */
	u8	ndid; /* 0x46 - number of device ids */
	u32	did[5]; /* 0x47 - 5b device id 1..5 */
	u8	rsvd5[0x9];
	/* Backlight Control */
	u8	blcs; /* 0x64 - Backlight Control possible */
	u8	brtl;
	u8	odds;
	u8	rsvd6[0x7];
	/* Ambient Light Sensors*/
	u8	alse; /* 0x6e - ALS enable */
	u8	alaf;
	u8	llow;
	u8	lhih;
	u8	rsvd7[0x6];
	/* EMA */
	u8	emae; /* 0x78 - EMA enable */
	u16	emap;
	u16	emal;
	u8	rsvd8[0x5];
	/* MEF */
	u8	mefe; /* 0x82 - MEF enable */
	u8	rsvd9[0x9];
	/* TPM support */
	u8	tpmp; /* 0x8c - TPM */
	u8	tpme;
	u8	rsvd10[8];
	/* SATA */
	u8	gtf0[7]; /* 0x96 - GTF task file buffer for port 0 */
	u8	gtf1[7];
	u8	gtf2[7];
	u8	idem;
	u8	idet;
	u8	rsvd11[7];
	/* IGD OpRegion (not implemented yet) */
	u32	aslb; /* 0xb4 - IGD OpRegion Base Address */
	u8	ibtt;
	u8	ipat;
	u8	itvf;
	u8	itvm;
	u8	ipsc;
	u8	iblc;
	u8	ibia;
	u8	issc;
	u8	i409;
	u8	i509;
	u8	i609;
	u8	i709;
	u8	idmm;
	u8	idms;
	u8	if1e;
	u8	hvco;
	u32	nxd[8];
	u8	rsvd12[8];
	/* Mainboard specific */
	u8	dock; /* 0xf0 - Docking Status */
	u8	bten;
	u8	rsvd13[14];
} __attribute__((packed)) global_nvs_t;

