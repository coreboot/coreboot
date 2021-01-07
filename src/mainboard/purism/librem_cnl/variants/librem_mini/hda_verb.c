/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x10ec0269,	/* Codec Vendor/Device ID: Realtek ALC293 */
	0x10ec0000,	/* Subsystem ID */
	11,		/* Number of entries */

	AZALIA_SUBVENDOR(0, 0x10ec0000),
	AZALIA_PIN_CFG(0, 0x12, 0x411111f0), /* NC */
	AZALIA_PIN_CFG(0, 0x14, 0x411111f0), /* NC */
	AZALIA_PIN_CFG(0, 0x15, 0x02211010), /* Jack analog out */
	AZALIA_PIN_CFG(0, 0x17, 0x411111f0), /* NC */
	AZALIA_PIN_CFG(0, 0x18, 0x02a11020), /* Jack analog mic */
	AZALIA_PIN_CFG(0, 0x19, 0x411111f0), /* NC */
	AZALIA_PIN_CFG(0, 0x1a, 0x411111f0), /* NC */
	AZALIA_PIN_CFG(0, 0x1b, 0x411111f0), /* NC */
	AZALIA_PIN_CFG(0, 0x1d, 0x411111f0), /* NC */
	AZALIA_PIN_CFG(0, 0x1e, 0x411111f0), /* NC */

	0x8086280b,	/* Codec Vendor/Device ID: Intel CannonPoint HDMI */
	0x80860101,	/* Subsystem ID */
	4,		/* Number of entries */

	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x07, 0x18560010),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
