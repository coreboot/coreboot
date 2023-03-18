/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SOUTHBRIDGE_INTEL_I82371EB_CHIP_H
#define SOUTHBRIDGE_INTEL_I82371EB_CHIP_H

#include <device/device.h>
#include <types.h>

struct southbridge_intel_i82371eb_config {
	bool ide0_enable;
	bool ide0_drive0_udma33_enable;
	bool ide0_drive1_udma33_enable;
	bool ide1_enable;
	bool ide1_drive0_udma33_enable;
	bool ide1_drive1_udma33_enable;
	bool ide_legacy_enable;
	bool usb_enable;
	bool gpo22_enable; /* GPO22/GPO23 (1) vs. XDIR#/XOE# (0) */
	int gpo22:1;
	int gpo23:1;
	/* acpi */
	u32 gpo; /* gpio output default */
	u8 lid_polarity;
	u8 thrm_polarity;
};

#endif /* SOUTHBRIDGE_INTEL_I82371EB_CHIP_H */
