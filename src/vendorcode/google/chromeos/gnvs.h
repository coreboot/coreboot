/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __VENDORCODE_GOOGLE_CHROMEOS_GNVS_H
#define __VENDORCODE_GOOGLE_CHROMEOS_GNVS_H

#include <stdint.h>

#define ACTIVE_ECFW_RO		0
#define ACTIVE_ECFW_RW		1

/*
 * chromeos_acpi_t portion of ACPI GNVS is assumed to live at
 * 0x100 - 0x1000.
 */
#define GNVS_CHROMEOS_ACPI_OFFSET 0x100

/* device_nvs_t is assumed to live directly after chromeos_acpi_t. */
#define GNVS_DEVICE_NVS_OFFSET 0x1000

typedef struct {
	/* ChromeOS specific */
	u32	vbt0;		// 00 boot reason
	u32	vbt1;		// 04 active main firmware
	u32	vbt2;		// 08 active ec firmware
	u16	vbt3;		// 0c CHSW
	u8	vbt4[256];	// 0e HWID
	u8	vbt5[64];	// 10e FWID
	u8	vbt6[64];	// 14e FRID - 275
	u32	vbt7;		// 18e active main firmware type
	u32	vbt8;		// 192 recovery reason
	u32	vbt9;		// 196 fmap base address
	u8	vdat[3072];	// 19a VDAT space filled by verified boot
	u32	vbt10;		// d9a smbios bios version
	u32	mehh[8];	// d9e management engine hash
	u32	ramoops_base;	// dbe ramoops base address
	u32	ramoops_len;	// dc2 ramoops length
	u32	vpd_ro_base;	// dc6 pointer to RO_VPD
	u32	vpd_ro_size;	// dca size of RO_VPD
	u32	vpd_rw_base;	// dce pointer to RW_VPD
	u32	vpd_rw_size;	// dd2 size of RW_VPD
	u8	pad[298];	// dd6-eff
} __packed chromeos_acpi_t;

void chromeos_init_chromeos_acpi(chromeos_acpi_t *init);
chromeos_acpi_t *chromeos_get_chromeos_acpi(void);

#endif
