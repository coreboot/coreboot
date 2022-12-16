/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x10ec0662, /* Codec Vendor / Device ID: Realtek ALC662 rev3 */
	0x17aa30bc, /* Subsystem ID */
	12,	    /* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x17aa30bc),
	AZALIA_PIN_CFG(0, 0x12, 0x40000000),
	AZALIA_PIN_CFG(0, 0x14, 0x01014020),
	AZALIA_PIN_CFG(0, 0x15, 0x99130110),
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19040),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19050),
	AZALIA_PIN_CFG(0, 0x1a, 0x0181304f),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214030),
	AZALIA_PIN_CFG(0, 0x1c, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, 0x4047c62b),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),

	0x80862809, /* Codec Vendor / Device ID: Intel Skylake HDMI */
	0x80860101, /* Subsystem ID */
	4,	    /* Number of 4 dword sets */
	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x05, 0x58560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560020),
	AZALIA_PIN_CFG(2, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[] = {};
AZALIA_ARRAY_SIZES;
