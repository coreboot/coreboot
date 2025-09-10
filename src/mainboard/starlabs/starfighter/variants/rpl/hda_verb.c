/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>
#include <device/azalia_codec/realtek.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0256,	/* Codec Vendor / Device ID: Realtek ALC256 */
	0x10ec1200,	/* Subsystem ID */
	30,		/* Number of verb entries */

	/* Reset Codec First */
	AZALIA_RESET(0x1),

	/* HDA Codec Subsystem ID */
	AZALIA_SUBVENDOR(0, 0x10ec1200),

	/* Pin Widget Verb-table */
	AZALIA_PIN_CFG(0, ALC256_DMIC12,	AZALIA_PIN_DESC(
							AZALIA_INTEGRATED,
							AZALIA_MOBILE_LID_INSIDE,
							AZALIA_MIC_IN,
							AZALIA_OTHER_DIGITAL,
							AZALIA_COLOR_UNKNOWN,
							AZALIA_NO_JACK_PRESENCE_DETECT,
							3,
							0
						)),
	AZALIA_PIN_CFG(0, ALC256_MIC2,		AZALIA_PIN_DESC(
							AZALIA_JACK,
							AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_RIGHT,
							AZALIA_MIC_IN,
							AZALIA_STEREO_MONO_1_8,
							AZALIA_BLACK,
							AZALIA_JACK_PRESENCE_DETECT,
							4,
							0
						)),
	/* Internal speakers are connected to LINE2 and external amps */
	AZALIA_PIN_CFG(0, ALC256_LINE2,		AZALIA_PIN_DESC(
							AZALIA_INTEGRATED,
							AZALIA_INTERNAL | AZALIA_FRONT,
							AZALIA_SPEAKER,
							AZALIA_OTHER_ANALOG,
							AZALIA_COLOR_UNKNOWN,
							AZALIA_NO_JACK_PRESENCE_DETECT,
							1,
							0
						)),
	AZALIA_PIN_CFG(0, ALC256_HP_OUT,	AZALIA_PIN_DESC(
							AZALIA_JACK,
							AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_RIGHT,
							AZALIA_HP_OUT,
							AZALIA_STEREO_MONO_1_8,
							AZALIA_BLACK,
							AZALIA_JACK_PRESENCE_DETECT,
							2,
							0
						)),
	AZALIA_PIN_CFG(0, ALC256_DMIC34,	AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC256_SPEAKERS,	AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC256_LINE1,		AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC256_PC_BEEP,	AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC256_SPDIF_OUT,	AZALIA_PIN_CFG_NC(0)),

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
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
