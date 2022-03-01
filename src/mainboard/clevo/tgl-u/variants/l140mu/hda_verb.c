/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* Realtek ALC293 */
	0x10ec0293, /* Vendor ID */
	0x155814a1, /* Subsystem ID */
	12, /* Number of entries */
	AZALIA_SUBVENDOR(0, 0x155814a1),

	/* Microphone (display lid), vendor value: 0x90a60130 */
	AZALIA_PIN_CFG(0, 0x12, AZALIA_PIN_DESC(
			INTEGRATED,
			LOCATION_OTHER,		/* vendor: SEPARATE_CHASSIS */
			SPECIAL7,		/* lid, vendor: NA */
			MIC_IN,
			OTHER_DIGITAL,
			COLOR_UNKNOWN,
			1,			/* no presence detect */
			3, 0)
	),

	/* Integrated speakers, vendor value: 0x90170110 */
	AZALIA_PIN_CFG(0, 0x14, AZALIA_PIN_DESC(
			INTEGRATED,
			LOCATION_OTHER,		/* vendor: SEPARATE_CHASSIS */
			BOTTOM,			/* vendor: NA */
			SPEAKER,
			OTHER_ANALOG,
			COLOR_UNKNOWN,
			1,			/* no presence detect */
			1, 0)
	),

	/* Headphones, vendor value: 0x02211020 */
	AZALIA_PIN_CFG(0, 0x15, AZALIA_PIN_DESC(
			JACK,
			EXTERNAL_PRIMARY_CHASSIS,
			RIGHT,			/* vendor: FRONT */
			HP_OUT,
			STEREO_MONO_1_8,
			BLACK,
			0,			/* has presence detect */
			2, 0)
	),

	/* ext. Microphone, vendor value: 0x411111f0, linux override: 0x01a1913c */
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_DESC(
			JACK,
			EXTERNAL_PRIMARY_CHASSIS,
			RIGHT,			/* vendor: REAR */
			MIC_IN,
			STEREO_MONO_1_8,
			BLACK,			/* vendor: PINK */
			1,			/* no separate presence detect */
			3, 12)
	),

	/* PCBEEP, vendor value: 0x41748245 */
	AZALIA_PIN_CFG(0, 0x1d, AZALIA_PIN_DESC(
			INTEGRATED,		/* vendor: NC */
			INTERNAL,		/* vendor: EXTERNAL_PRIMARY_CHASSIS */
			NA,			/* vendor: REAR */
			DEVICE_OTHER,		/* vendor: MODEM_HANDSET_SIDE */
			OTHER_ANALOG,		/* vendor: RCA */
			COLOR_UNKNOWN,		/* vendor: PURPLE */
			1,			/* no presence detect, vendor: 2 */
			4, 5)
	),

	AZALIA_PIN_CFG(0, 0x13, 0x40000000),	/* NC, but different from 0x411111f0 */
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
