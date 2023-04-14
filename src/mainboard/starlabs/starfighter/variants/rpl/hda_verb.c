/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0256, /* Codec Vendor / Device ID: Realtek ALC256 */
	0x10ec1200, /* Subsystem ID */
	32,	/* Number of jacks (NID entries) */

	/* Reset Codec First */
	AZALIA_RESET(0x1),

	/* HDA Codec Subsystem ID: 0x10EC1200 */
	AZALIA_SUBVENDOR(0, 0x10ec1200),

	AZALIA_PIN_CFG(0, 0x01, 0x00000000),
	AZALIA_PIN_CFG(0, 0x12, 0x90a60130),
	AZALIA_PIN_CFG(0, 0x13, 0x40000000),
	AZALIA_PIN_CFG(0, 0x14, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x18, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x19, 0x04a19040),
	AZALIA_PIN_CFG(0, 0x1a,	0x411111f0),
	AZALIA_PIN_CFG(0, 0x1b, 0x90170110),
	AZALIA_PIN_CFG(0, 0x1d, 0x40689a6d),
	AZALIA_PIN_CFG(0, 0x1e, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x21, 0x04214020),

	/* Reset and Set LDO3 output to 1.2V */
	0x0205001a,
	0x0204c003,
	0x02050019,
	0x02040f52,
	/* ALC256 Relay 1 */
	0x0205001b,
	0x0204064b,
	0x02050045,
	0x0204b089,
	/* ALC256 Relay 2 */
	0x02050046,
	0x02040004,
	0x02050040,
	0x02048800,

	/*
	 * Equalizer:
	 *
	 * AGC
	 * Threshold:	- 6.00 dB
	 * Front Boost:	+ 6.00 dB
	 * Post Boost:	+ 6.00 dB
	 *
	 * Low Pass Filter
	 * Boost Gain:	Enabled
	 * BW:		200Hz
	 * Gain:	+ 4.00 dB
	 *
	 * Band Pass Filter 1
	 * Fc:		240Hz
	 * BW:		400Hz
	 * Gain:	- 4.00 dB
	 *
	 * Band Pass Filter 2
	 * Fc:		16000Hz
	 * BW:		1000Hz
	 * Gain:	+ 12.00 dB
	 *
	 * High Pass Filter
	 * Boost Gain:	Enabled
	 * BW:		200Hz
	 * Gain:	- 4.00 dB
	 *
	 * Class D Amp
	 * Power:	2.5W
	 * Resistance:	4ohms
	 *
	 * EQ Output
	 * Left:	+ 0.00 dB
	 * Right:	+ 0.00 dB
	 *
	 * VARQ
	 * Q:		0.707
	 */

	0x05350000,
	0x053404DA,
	0x0535001d,
	0x05340800,

	0x0535001e,
	0x05340800,
	0x05350003,
	0x05341F7A,

	0x05350004,
	0x0534FA18,
	0x0535000F,
	0x0534C295,

	0x05350010,
	0x05341D73,
	0x05350011,
	0x0534FA18,

	0x05350012,
	0x05341E08,
	0x05350013,
	0x05341C10,

	0x05350014,
	0x05342FB2,
	0x0535001B,
	0x05341F2C,

	0x0535001C,
	0x0534095C,
	0x05450000,
	0x05440000,

	0x0545001d,
	0x05440800,
	0x0545001e,
	0x05440800,

	0x05450003,
	0x05441F7A,
	0x05450004,
	0x0544FA18,

	0x0545000F,
	0x0544C295,
	0x05450010,
	0x05441D73,

	0x05450011,
	0x0544FA18,
	0x05450012,
	0x05441E08,

	0x05450013,
	0x05441C10,
	0x05450014,
	0x05442FB2,

	0x0545001B,
	0x05441F2C,
	0x0545001C,
	0x0544095C,

	0x05350000,
	0x0534C4DA,
	0x02050038,
	0x02044901,

	0x02050013,
	0x0204422F,
	0x02050016,
	0x02044E50,

	0x02050012,
	0x0204EBC4,
	0x02050020,
	0x020451FF,

	0x80862815, /* Codec Vendor / Device ID: Intel */
	0x80860101, /* Subsystem ID */
	10,	    /* Number of 4 dword sets */

	AZALIA_SUBVENDOR(2, 0x80860101),

	AZALIA_PIN_CFG(2, 0x04, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x08, 0x18561010),
	AZALIA_PIN_CFG(2, 0x0a, 0x18561010),
	AZALIA_PIN_CFG(2, 0x0b, 0x18561010),
	AZALIA_PIN_CFG(2, 0x0c, 0x18561010),
	AZALIA_PIN_CFG(2, 0x0d, 0x18561010),
	AZALIA_PIN_CFG(2, 0x0e, 0x18561010),
	AZALIA_PIN_CFG(2, 0x0f, 0x18560010),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
