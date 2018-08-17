/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_NVS_H_
#define _SOC_NVS_H_

#include <commonlib/helpers.h>
#include <compiler.h>
#include <rules.h>
#include <vendorcode/google/chromeos/gnvs.h>

typedef struct global_nvs_t {
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
	u8      pcnt; /* 0x0b - Processor Count */
	u8	ppcm; /* 0x0c - Max PPC State */
	u8	tmps; /* 0x0d - Temperature Sensor ID */
	u8	tlvl; /* 0x0e - Throttle Level Limit */
	u8	flvl; /* 0x0f - Current FAN Level */
	u8	tcrt; /* 0x10 - Critical Threshold */
	u8	tpsv; /* 0x11 - Passive Threshold */
	u8	tmax; /* 0x12 - CPU Tj_max */
	u8	s5u0; /* 0x13 - Enable USB in S5 */
	u8	s3u0; /* 0x14 - Enable USB in S3 */
	u8	s33g; /* 0x15 - Enable 3G in S3 */
	u8	lids; /* 0x16 - LID State */
	u8	pwrs; /* 0x17 - AC Power State */
	u32	cmem; /* 0x18 - 0x1b - CBMEM TOC */
	u32	cbmc; /* 0x1c - 0x1f - coreboot Memory Console */
	u64	pm1i; /* 0x20 - 0x27 - PM1 wake status bit */
	u64	gpei; /* 0x28 - 0x2f - GPE wake status bit */
	u8	dpte; /* 0x30 - Enable DPTF */
	u64	nhla; /* 0x31 - NHLT Address */
	u32	nhll; /* 0x39 - NHLT Length */
	u16	cid1; /* 0x3d - Wifi Country Identifier */
	u16	u2we; /* 0x3f - USB2 Wake Enable Bitmap */
	u8	u3we; /* 0x41 - USB3 Wake Enable Bitmap */
	u8	uior; /* 0x42 - UART debug controller init on S3 resume */
	u8	ecps; /* 0x43 - SGX Enabled status */
	u64	emna; /* 0x44 - 0x4B EPC base address */
	u64	elng; /* 0x4C - 0x53 EPC Length */
	u8	rsvd[96];

	/* IGD OpRegion */
	u32	aslb; /* 0xb4 - IGD OpRegion Base Address */
	u8	ibtt; /* 0xb8 - IGD boot type */
	u8	ipat; /* 0xb9 - IGD panel type */
	u8	itvf; /* 0xba - IGD TV format */
	u8	itvm; /* 0xbb - IGD TV minor format */
	u8	ipsc; /* 0xbc - IGD Panel Scaling */
	u8	iblc; /* 0xbd - IGD BLC configuration */
	u8	ibia; /* 0xbe - IGD BIA configuration */
	u8	issc; /* 0xbf - IGD SSC configuration */
	u8	i409; /* 0xc0 - IGD 0409 modified settings */
	u8	i509; /* 0xc1 - IGD 0509 modified settings */
	u8	i609; /* 0xc2 - IGD 0609 modified settings */
	u8	i709; /* 0xc3 - IGD 0709 modified settings */
	u8	idmm; /* 0xc4 - IGD Power Conservation */
	u8	idms; /* 0xc5 - IGD DVMT memory size */
	u8	if1e; /* 0xc6 - IGD Function 1 Enable */
	u8	hvco; /* 0xc7 - IGD HPLL VCO */
	u32	nxd[8]; /* 0xc8 - IGD next state DIDx for _DGS */
	u8	isci; /* 0xe8 - IGD SMI/SCI mode (0: SCI) */
	u8	pavp; /* 0xe9 - IGD PAVP data */
	u8	rsvd12; /* 0xea - rsvd */
	u8	oscc; /* 0xeb - PCIe OSC control */
	u8	npce; /* 0xec - native pcie support */
	u8	plfl; /* 0xed - platform flavor */
	u8	brev; /* 0xee - board revision */
	u8	dpbm; /* 0xef - digital port b mode */
	u8	dpcm; /* 0xf0 - digital port c mode */
	u8	dpdm; /* 0xf1 - digital port c mode */
	u8	alfp; /* 0xf2 - active lfp */
	u8	imon; /* 0xf3 - current graphics turbo imon value */
	u8	mmio; /* 0xf4 - 64bit mmio support */

	u8	unused[11];

	/* ChromeOS specific (0x100 - 0xfff) */
	chromeos_acpi_t chromeos;
} __packed global_nvs_t;
check_member(global_nvs_t, chromeos, GNVS_CHROMEOS_ACPI_OFFSET);

#endif
