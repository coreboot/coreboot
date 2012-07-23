/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2011 Google Inc
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

#include "vendorcode/google/chromeos/gnvs.h"
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
	/* Thermal policy */
	u8	tlvl; /* 0x11 - Throttle Level Limit */
	u8	flvl; /* 0x12 - Current FAN Level */
	u8	tcrt; /* 0x13 - Critical Threshold */
	u8	tpsv; /* 0x14 - Passive Threshold */
	u8	tmax; /* 0x15 - CPU Tj_max */
	u8	f0of; /* 0x16 - FAN 0 OFF Threshold */
	u8	f0on; /* 0x17 - FAN 0 ON Threshold */
	u8	f0pw; /* 0x18 - FAN 0 PWM value */
	u8	f1of; /* 0x19 - FAN 1 OFF Threshold */
	u8	f1on; /* 0x1a - FAN 1 ON Threshold */
	u8	f1pw; /* 0x1b - FAN 1 PWM value */
	u8	f2of; /* 0x1c - FAN 2 OFF Threshold */
	u8	f2on; /* 0x1d - FAN 2 ON Threshold */
	u8	f2pw; /* 0x1e - FAN 2 PWM value */
	u8	f3of; /* 0x1f - FAN 3 OFF Threshold */
	u8	f3on; /* 0x20 - FAN 3 ON Threshold */
	u8	f3pw; /* 0x21 - FAN 3 PWM value */
	u8	f4of; /* 0x22 - FAN 4 OFF Threshold */
	u8	f4on; /* 0x23 - FAN 4 ON Threshold */
	u8	f4pw; /* 0x24 - FAN 4 PWM value */
	u8	tmps; /* 0x25 - Temperature Sensor ID */
	u8	rsvd3[2];
	/* Processor Identification */
	u8	apic; /* 0x28 - APIC enabled */
	u8	mpen; /* 0x29 - MP capable/enabled */
	u8	pcp0; /* 0x2a - PDC CPU/CORE 0 */
	u8	pcp1; /* 0x2b - PDC CPU/CORE 1 */
	u8	ppcm; /* 0x2c - Max. PPC state */
	u8      pcnt; /* 0x2d - Processor Count */
	u8	rsvd4[4];
	/* Super I/O & CMOS config */
	u8	natp; /* 0x32 - SIO type */
	u8	s5u0; /* 0x33 - Enable USB0 in S5 */
	u8	s5u1; /* 0x34 - Enable USB1 in S5 */
	u8	s3u0; /* 0x35 - Enable USB0 in S3 */
	u8	s3u1; /* 0x36 - Enable USB1 in S3 */
	u8	s33g; /* 0x37 - Enable S3 in 3G */
	u32	cmem; /* 0x38 - CBMEM TOC */
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
	/* ChromeOS specific (starts at 0xf0)*/
	chromeos_acpi_t chromeos;
} __attribute__((packed)) global_nvs_t;

