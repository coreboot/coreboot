/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x10ec0887,	/* Codec Vendor / Device ID: Realtek ALC887 */
	0x1458a002,	/* Subsystem ID */
	15,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(2, 0x1458a002),
	AZALIA_PIN_CFG(2, 0x11, 0x411110f0),
	AZALIA_PIN_CFG(2, 0x12, 0x411111f0),
	AZALIA_PIN_CFG(2, 0x14, 0x01014410),
	AZALIA_PIN_CFG(2, 0x15, 0x411111f0),
	AZALIA_PIN_CFG(2, 0x16, 0x411111f0),
	AZALIA_PIN_CFG(2, 0x17, 0x411111f0),
	AZALIA_PIN_CFG(2, 0x18, 0x01a19c20),
	AZALIA_PIN_CFG(2, 0x19, 0x02a19c30),
	AZALIA_PIN_CFG(2, 0x1a, 0x0181342f),
	AZALIA_PIN_CFG(2, 0x1b, 0x02214c1f),
	AZALIA_PIN_CFG(2, 0x1c, 0x411111f0),
	AZALIA_PIN_CFG(2, 0x1d, 0x4004c601),
	AZALIA_PIN_CFG(2, 0x1e, 0x411111f0),
	AZALIA_PIN_CFG(2, 0x1f, 0x411111f0),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
