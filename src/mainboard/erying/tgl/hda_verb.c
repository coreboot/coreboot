/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* Realtek ALC897 */
	0x10ec0897, /* Vendor ID */
	0x10ec3000, /* Subsystem ID */
	15, /* Number of entries */
	AZALIA_SUBVENDOR(0, 0x10ec3000),
	AZALIA_PIN_CFG(0, 0x11, 0x40000000),
	AZALIA_PIN_CFG(0, 0x12, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x14, 0x01014020),
	AZALIA_PIN_CFG(0, 0x15, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x16, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x17, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19030),
	AZALIA_PIN_CFG(0, 0x19, 0x02a1903f),
	AZALIA_PIN_CFG(0, 0x1a, 0x01813040),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214010),
	AZALIA_PIN_CFG(0, 0x1c, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x4024c601),
	AZALIA_PIN_CFG(0, 0x1e, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1f, 0x411111f0),

#if !CONFIG(SOC_INTEL_DISABLE_IGD)
	/* Tigerlake HDMI */
	0x80862812, /* Vendor ID */
	0x80860101, /* Subsystem ID */
	2, /* Number of entries */
	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x04, 0x18560010),
#endif
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
