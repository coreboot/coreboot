/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOUTHBRIDGE_INTEL_LYNXPOINT_NVS_H
#define SOUTHBRIDGE_INTEL_LYNXPOINT_NVS_H

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
	u8	unused_was_apic; /* 0x28 - APIC enabled */
	u8	unused_was_mpen; /* 0x29 - MP capable/enabled */
	u8	pcp0; /* 0x2a - PDC CPU/CORE 0 */
	u8	pcp1; /* 0x2b - PDC CPU/CORE 1 */
	u8	ppcm; /* 0x2c - Max. PPC state */
	u8      unused_was_pcnt; /* 0x2d - Processor Count */
	u8	rsvd4[4];
	/* Super I/O & CMOS config */
	u8	natp; /* 0x32 - SIO type */
	u8	s5u0; /* 0x33 - Enable USB0 in S5 */
	u8	s5u1; /* 0x34 - Enable USB1 in S5 */
	u8	s3u0; /* 0x35 - Enable USB0 in S3 */
	u8	s3u1; /* 0x36 - Enable USB1 in S3 */
	u8	s33g; /* 0x37 - Enable S3 in 3G */
	u32	obsolete_cmem; /* 0x38 - CBMEM TOC */
	/* Integrated Graphics Device */
	u8	igds; /* 0x3c - IGD state */
	u8	tlst; /* 0x3d - Display Toggle List Pointer */
	u8	cadl; /* 0x3e - currently attached devices */
	u8	padl; /* 0x3f - previously attached devices */
	u8	rsvd14[27];
	/* TPM support */
	u8	tpmp; /* 0x5b - TPM Present */
	u8	tpme; /* 0x5c - TPM Enable */
	u8	rsvd5[3];
	/* LynxPoint Serial IO device BARs */
	u32	s0b[8]; /* 0x60 - 0x7f - BAR0 */
	u32	s1b[8]; /* 0x80 - 0x9f - BAR1 */
	u32	cbmc;   /* 0xa0 - 0xa3 - coreboot memconsole */

	/* Required for future unified acpi_save_wake_source. */
	u32	pm1i;
	u32	gpei;
};

#endif /* SOUTHBRIDGE_INTEL_LYNXPOINT_NVS_H */
