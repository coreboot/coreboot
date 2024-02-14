/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* Alderlake-P HDMI */
	0x8086281c, /* Vendor ID */
	0x80860101, /* Subsystem ID */
	7, /* Number of entries */
	AZALIA_SUBVENDOR(2, 0x80860101),
	0x00278111,
	0x00278111,
	0x00278111,
	0x00278111,
	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560020),
	AZALIA_PIN_CFG(2, 0x07, 0x18560030),
	AZALIA_PIN_CFG(2, 0x08, 0x18560040),
	0x00278100,
	0x00278100,
	0x00278100,
	0x00278100
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
