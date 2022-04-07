/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* Realtek ALC897 */
	0x10ec0897, /* Vendor ID */
	0x14629d25, /* Subsystem ID */
	15, /* Number of entries */
	AZALIA_SUBVENDOR(0, 0x14629d25),
	AZALIA_PIN_CFG(0, 0x11, 0x4037d540),
	AZALIA_PIN_CFG(0, 0x12, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x14, 0x01014010),
	AZALIA_PIN_CFG(0, 0x15, 0x01011012),
	AZALIA_PIN_CFG(0, 0x16, 0x01016011),
	AZALIA_PIN_CFG(0, 0x17, 0x01012014),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19030),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19040),
	AZALIA_PIN_CFG(0, 0x1a, 0x0181303f),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214020),
	AZALIA_PIN_CFG(0, 0x1c, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x402af66b),
	AZALIA_PIN_CFG(0, 0x1e, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1f, 0x411111f0),

	/* Alderlake HDMI */
	0x80862815, /* Vendor ID */
	0x80860101, /* Subsystem ID */
	2, /* Number of entries */
	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x04, 0x18560010),
};


const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
