/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BAYTRAIL_NVS_H_
#define _BAYTRAIL_NVS_H_

#include <stdint.h>

struct __packed global_nvs {
	/* Miscellaneous */
	u16	unused_was_osys; /* 0x00 - Operating System */
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
	u8	unused_was_pwrs; /* 0x10 - Power state (AC = 1) */
	u8	unused_was_pcnt; /* 0x11 - Processor Count */
	u8	tpmp; /* 0x12 - TPM Present and Enabled */
	u8	tlvl; /* 0x13 - Throttle Level */
	u8	ppcm; /* 0x14 - Maximum P-state usable by OS */
	u32	pm1i; /* 0x15 - System Wake Source - PM1 Index */
	u32	gpei; /* 0x19 - GPE Wake Source */
	u8	rsvd1[3];

	/* Device Config */
	u8	s5u0; /* 0x20 - Enable USB0 in S5 */
	u8	s5u1; /* 0x21 - Enable USB1 in S5 */
	u8	s3u0; /* 0x22 - Enable USB0 in S3 */
	u8	s3u1; /* 0x23 - Enable USB1 in S3 */
	u8	tact; /* 0x24 - Thermal Active trip point */
	u8	tpsv; /* 0x25 - Thermal Passive trip point */
	u8	tcrt; /* 0x26 - Thermal Critical trip point */
	u8	dpte; /* 0x27 - Enable DPTF */
	u8	rsvd2[8];

	/* Base Addresses */
	u32	obsolete_cmem; /* 0x30 - CBMEM TOC */
	u32	tolm; /* 0x34 - Top of Low Memory */
	u32	cbmc; /* 0x38 - coreboot memconsole */
};

#endif /* _BAYTRAIL_NVS_H_ */
