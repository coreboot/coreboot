/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0269,	/* Codec Vendor / Device ID: Realtek ALC269 */
	0x19910269,	/* Subsystem ID */
	12,		/* Number of jacks (NID entries) */

	AZALIA_RESET(1),
	AZALIA_SUBVENDOR(0, 0x19910269),
	AZALIA_PIN_CFG(0, 0x12, 0x40000000),
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x18, 0x04a11020),
	AZALIA_PIN_CFG(0, 0x19, 0x90a70130),
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, 0x40569d05),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x21, 0x0421101f),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
