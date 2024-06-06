/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x10ec0233,	/* Codec Vendor / Device ID: Realtek ALC233 */
	0x17aa30d0,	/* Subsystem ID */
	11,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(1, 0x17aa30d0),
	AZALIA_PIN_CFG(1, 0x12, 0x40000000),
	AZALIA_PIN_CFG(1, 0x14, 0x90170110),
	AZALIA_PIN_CFG(1, 0x17, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(1, 0x18, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(1, 0x19, 0x02a11030),
	AZALIA_PIN_CFG(1, 0x1a, 0x02a11040),
	AZALIA_PIN_CFG(1, 0x1b, 0x01011020),
	AZALIA_PIN_CFG(1, 0x1d, 0x40400001),
	AZALIA_PIN_CFG(1, 0x1e, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(1, 0x21, 0x0221101f),

	0x80862809,	/* Codec Vendor / Device ID: Intel Skylake HDMI */
	0x80860101,	/* Subsystem ID */
	4,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560020),
	AZALIA_PIN_CFG(2, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[] = {
};
AZALIA_ARRAY_SIZES;
