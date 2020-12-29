/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_NVS_H_
#define _SOC_NVS_H_

#include <stdint.h>
#include <commonlib/helpers.h>
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
	u64	a4gb; /* 0x54 - 0x5B Base of above 4GB MMIO Resource */
	u64	a4gs; /* 0x5C - 0x63 Length of above 4GB MMIO Resource */
	u8	rsvd[156];

	/* ChromeOS specific (0x100 - 0xfff) */
	chromeos_acpi_t chromeos;
};

check_member(global_nvs, chromeos, GNVS_CHROMEOS_ACPI_OFFSET);

#endif
