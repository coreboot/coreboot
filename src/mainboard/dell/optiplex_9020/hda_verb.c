/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x10ec0280, /* Realtek ALC3220 */
	0x102805a5, /* Subsystem ID */
	13,	 /* Number of entries */
	AZALIA_SUBVENDOR(0, 0x102805a5),
	AZALIA_PIN_CFG(0, 0x12, 0x4008c000),
	AZALIA_PIN_CFG(0, 0x13, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),
	AZALIA_PIN_CFG(0, 0x15, 0x0221401f),
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x18, 0x01a13040),
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1a, 0x02a19030),
	AZALIA_PIN_CFG(0, 0x1b, 0x01014020),
	AZALIA_PIN_CFG(0, 0x1d, 0x40400001),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
