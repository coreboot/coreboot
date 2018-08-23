/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

#ifndef __VENDORCODE_GOOGLE_CHROMEOS_GNVS_H
#define __VENDORCODE_GOOGLE_CHROMEOS_GNVS_H

#include <compiler.h>

#define BOOT_REASON_OTHER	0
#define BOOT_REASON_S3DIAG	9

#define CHSW_RECOVERY_X86	(1 << 1)
#define CHSW_RECOVERY_EC	(1 << 2)
#define CHSW_DEVELOPER_SWITCH	(1 << 5)
#define CHSW_FIRMWARE_WP_DIS	(1 << 9)

#define ACTIVE_MAINFW_RECOVERY	0
#define ACTIVE_MAINFW_RW_A	1
#define ACTIVE_MAINFW_RW_B	2

#define ACTIVE_MAINFW_TYPE_RECOVERY	0
#define ACTIVE_MAINFW_TYPE_NORMAL	1
#define ACTIVE_MAINFW_TYPE_DEVELOPER	2

#define RECOVERY_REASON_NONE	0
#define RECOVERY_REASON_ME	1
// TODO(reinauer) other recovery reasons?

#define ACTIVE_ECFW_RO		0
#define ACTIVE_ECFW_RW		1

/*
 * chromeos_acpi_t portion of ACPI GNVS is assumed to live at
 * 0x100 - 0x1000.  When defining global_nvs_t, use check_member
 * to ensure that it is properly aligned:
 *
 *   check_member(global_nvs_t, chromeos, GNVS_CHROMEOS_ACPI_OFFSET);
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
	u8	pad[314];	// dc6-eff
} __packed chromeos_acpi_t;

void chromeos_init_chromeos_acpi(chromeos_acpi_t *init);
void chromeos_set_me_hash(u32*, int);
chromeos_acpi_t *chromeos_get_chromeos_acpi(void);

#endif
