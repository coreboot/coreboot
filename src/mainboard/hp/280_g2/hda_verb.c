/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0221,	/* Codec Vendor / Device ID: Realtek ALC221 */
	0x103c2b5e,	/* Subsystem ID */
	11,		/* Number of jacks */
	AZALIA_SUBVENDOR(0, 0x103c2b5e),
	AZALIA_PIN_CFG(0, 0x12, 0x40000000),
	AZALIA_PIN_CFG(0, 0x14, 0x01014010),
	AZALIA_PIN_CFG(0, 0x17, 0x90170120),
	AZALIA_PIN_CFG(0, 0x18, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x19, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1a, 0x01813030),
	AZALIA_PIN_CFG(0, 0x1b, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x4044c301),
	AZALIA_PIN_CFG(0, 0x1e, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x21, 0x0221101f),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
