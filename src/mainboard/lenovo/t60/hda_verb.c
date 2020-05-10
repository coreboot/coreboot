/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x11d41981,	/* Codec Vendor / Device ID: Analog Devices AD1981 */
	0x17aa2025,	/* Subsystem ID */
	11,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x17aa2025),
	AZALIA_PIN_CFG(0, 0x05, 0xc3014110),
	AZALIA_PIN_CFG(0, 0x06, 0x4221401f),
	AZALIA_PIN_CFG(0, 0x07, 0x591311f0),
	AZALIA_PIN_CFG(0, 0x08, 0xc3a15020),
	AZALIA_PIN_CFG(0, 0x09, 0x41813021),
	AZALIA_PIN_CFG(0, 0x0a, 0x014470f0),
	AZALIA_PIN_CFG(0, 0x16, 0x59f311f0),
	AZALIA_PIN_CFG(0, 0x17, 0x59931122),
	AZALIA_PIN_CFG(0, 0x18, 0x41a19023),
	AZALIA_PIN_CFG(0, 0x19, 0x9933e12e)
};
const u32 pc_beep_verbs[0] = {};
AZALIA_ARRAY_SIZES;
