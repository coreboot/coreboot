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
	u8      pcnt; /* 0x11 - Processor Count */
	u8	tpmp; /* 0x12 - TPM Present and Enabled */
	u8	rsvd1[13];

	/* Device Config */
	u8	s5u0; /* 0x20 - Enable USB0 in S5 */
	u8	s5u1; /* 0x21 - Enable USB1 in S5 */
	u8	s3u0; /* 0x22 - Enable USB0 in S3 */
	u8	s3u1; /* 0x23 - Enable USB1 in S3 */
	u8	rsvd2[12];

	/* Base Addresses */
	u32	cmem; /* 0x30 - CBMEM TOC */
	u32	tolm; /* 0x34 - Top of Low Memory */
	u32	cbmc; /* 0x38 - coreboot memconsole */
	u8	rsvd5[39];

	/* Serial IO device BARs */
	u32	s0b[8]; /* 0x60 - 0x7f - BAR0 */
	u32	s1b[8]; /* 0x80 - 0x9f - BAR1 */
	u8	rsvd6[95];

	/* ChromeOS specific (starts at 0x100)*/
	chromeos_acpi_t chromeos;
} __attribute__((packed)) global_nvs_t;

#ifdef __SMM__
/* Used in SMM to find the ACPI GNVS address */
global_nvs_t *smm_get_gnvs(void);
#endif
