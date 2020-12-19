/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_NVS_H
#define SOC_INTEL_COMMON_BLOCK_NVS_H

#include <commonlib/helpers.h>
#include <vendorcode/google/chromeos/gnvs.h>

struct __packed global_nvs {
	/* Miscellaneous */
	u16	osys; /* 0x00 - 0x01 Operating System */
	u8	smif; /* 0x02 - SMI function call ("TRAP") */
	u8	pcnt; /* 0x03 - Processor Count */
	u8	ppcm; /* 0x04 - Max PPC State */
	u8	tlvl; /* 0x05 - Throttle Level Limit */
	u8	lids; /* 0x06 - LID State */
	u8	pwrs; /* 0x07 - AC Power State */
	u32	cbmc; /* 0x08 - 0xb coreboot Memory Console */
	u64	pm1i; /* 0x0c - 0x13 PM1 wake status bit */
	u64	gpei; /* 0x14 - 0x1b GPE wake status bit */
	u8	dpte; /* 0x1c - Enable DPTF */
	u64	nhla; /* 0x1d - 0x24 NHLT Address */
	u32	nhll; /* 0x25 - 0x28 NHLT Length */
	u16	cid1; /* 0x29 - 0x2a Wifi Country Identifier */
	u16	u2we; /* 0x2b - 0x2c USB2 Wake Enable Bitmap */
	u16	u3we; /* 0x2d - 0x2e USB3 Wake Enable Bitmap */
	u8	uior; /* 0x2f - UART debug controller init on S3 resume */
	u64	a4gb; /* 0x30 - 0x37 Base of above 4GB MMIO Resource */
	u64	a4gs; /* 0x38 - 0x3f Length of above 4GB MMIO Resource */
	u8	unused[192];

	/* ChromeOS specific (0x100 - 0xfff) */
	chromeos_acpi_t	chromeos;
};

check_member(global_nvs, chromeos, GNVS_CHROMEOS_ACPI_OFFSET);

#endif
