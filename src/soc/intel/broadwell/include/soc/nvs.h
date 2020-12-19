/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_NVS_H_
#define _BROADWELL_NVS_H_

#include <commonlib/helpers.h>
#include <soc/device_nvs.h>
#include <vendorcode/google/chromeos/gnvs.h>

struct __packed global_nvs {
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
	u32	obsolete_cmem; /* 0x18 - 0x1b - CBMEM TOC */
	u32	cbmc; /* 0x1c - 0x1f - coreboot Memory Console */
	u64	pm1i; /* 0x20 - 0x27 - PM1 wake status bit */
	u64	gpei; /* 0x28 - 0x2f - GPE wake status bit */
	u8	unused1[132]; /* 0x30 - 0xb3 - unused */

	u8	unused2[76];

	/* ChromeOS specific (0x100 - 0xfff) */
	chromeos_acpi_t chromeos;

	/* Device specific (0x1000) */
	device_nvs_t dev;
};

check_member(global_nvs, chromeos, GNVS_CHROMEOS_ACPI_OFFSET);

#endif
