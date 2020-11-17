/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x10ec0256,	/* Codec Vendor/Device ID: Realtek ALC256 */
	0x1b50580a,	/* Subsystem ID */
	12,		/* Number of entries */

	AZALIA_SUBVENDOR(0, 0x1b50580a),

	/* Widget node 0x01 */
	0x0017ff00,
	0x0017ff00,
	0x0017ff00,
	0x0017ff00,

	AZALIA_PIN_CFG(0, 0x12, 0x90a60140), /* Front digital mic */
	AZALIA_PIN_CFG(0, 0x13, 0x411111f0), /* NC */
	AZALIA_PIN_CFG(0, 0x14, 0x90170110), /* Internal speakers */
	AZALIA_PIN_CFG(0, 0x18, 0x411111f0), /* NC */
	AZALIA_PIN_CFG(0, 0x19, 0x04a11030), /* Jack analog mic */
	AZALIA_PIN_CFG(0, 0x1a, 0x411111f0), /* NC */
	AZALIA_PIN_CFG(0, 0x1b, 0x411111f0), /* NC */
	AZALIA_PIN_CFG(0, 0x1d, 0x411111f0), /* NC */
	AZALIA_PIN_CFG(0, 0x1e, 0x411111f0), /* NC */
	AZALIA_PIN_CFG(0, 0x21, 0x04211020), /* Jack analog out */

	/* Hidden SW reset */
	0x0205001a,
	0x0204c003,
	0x0205001a,
	0x02048003,

	/* Class D power on reset control */
	0x0205003c,
	0x02040354,
	0x0205003c,
	0x02040314,

	/* LDO3 output set to 1.2V */
	0x0205001b,
	0x02040a4b,
	0x02050019,
	0x02040e12,

	/* SPK power */
	0x02050038,
	0x02046901,
	0x02050007,
	0x02040200,

	0x8086280b,	/* Codec Vendor/Device ID: Intel CometPoint HDMI */
	0x80860101,	/* Subsystem ID */
	4,		/* Number of entries */

	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x07, 0x18560010),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
