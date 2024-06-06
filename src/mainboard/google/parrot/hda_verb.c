/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Parrot audio ports:
 * ALC269:
 *  out:
 *   Internal Speaker: PORT D (0x14)
 *   Headphone: PORT A - (0x21) (SenseA)
 *
 *  in:
 *   Mic2: PORT F (0x19) (SenseB)
 *   Line2 (internal Mic): PORT E (0x1B)
 *   PCBeep
 *
 *  HDMI PatherPoint
 */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0269,	// Codec Vendor / Device ID: Realtek ALC269
	0x10250742,	// Subsystem ID
	0x0000000E,	// Number of jacks (NID entries)

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x10250742 */
	AZALIA_SUBVENDOR(0, 0x10250742),

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x12) DMIC */
	AZALIA_PIN_CFG(0, 0x12, AZALIA_PIN_CFG_NC(0)),

	/* Pin Complex (NID 0x14) SPKR-OUT PORTD */
	// group 1, front left/right
	// no connector, no jack detect
	// speaker out, analog
	// fixed function, internal
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),

	/* Pin Complex (NID 0x17)  */
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(0)),

	/* Pin Complex (NID 0x18)  MIC1 PORTB */
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_CFG_NC(0)),

	/* Pin Complex (NID 0x19)  MIC2 PORTF */
	// group 2, cap 1
	// black, jack detect
	// mic in, analog
	// connector, right panel
	AZALIA_PIN_CFG(0, 0x19, 0x04a71021),

	/* Pin Complex (NID 0x1A)  LINE1 PORTC */
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)),

	/* Pin Complex (NID 0x1B)  LINE2 PORTE */
	// group 2, cap 0
	// no connector, no jack detect
	// mic in, analog
	// fixed function, internal
	AZALIA_PIN_CFG(0, 0x1b, 0x90a70120),

	/* Pin Complex (NID 0x1d)  PCBeep */
	// eapd low on ex-amp, laptop, custom enable
	// mute spkr on hpout
	// pcbeep enable, checksum
	// no physical, internal
	AZALIA_PIN_CFG(0, 0x1d, 0x4015812d),

	/* Pin Complex (NID 0x1E)  SPDIF-OUT */
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),

	/* Pin Complex (NID 0x21) HPOUT PORTA? */
	// group1,
	// black, jack detect
	// HPOut, 1/8 stereo
	// connector, right panel
	AZALIA_PIN_CFG(0, 0x21, 0x0421101f),

	/* Undocumented speaker output volume settings from Compal and Realtek */
	/* Widget node 0x20 */
	0x02050011,
	0x02040710,
	0x02050012,
	0x02041901,

	0x02050002,
	0x0204AAB8,
	0x0205000D,
	0x02044440,

	0x02050008,
	0x02040300,
	0x02050017,
	0x020400AF,

	/* --- Next Codec --- */

	/* coreboot specific header */
	0x80862806,	// Codec Vendor / Device ID: Intel PantherPoint HDMI
	0x80860101,	// Subsystem ID
	0x00000004,	// Number of jacks

	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x80860101 */
	AZALIA_SUBVENDOR(3, 0x80860101),

	/* Pin Complex (NID 0x05) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(3, 0x05, 0x18560010),

	/* Pin Complex (NID 0x06) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(3, 0x06, 0x18560020),

	/* Pin Complex (NID 0x07) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[] = {
	0x00170500,	/* power up everything (codec, dac, adc, mixers)  */
	0x01470740,	/* enable speaker out */
	0x01470c02,	/* set speaker EAPD pin */
	0x0143b01f,	/* unmute speaker */
	0x00c37100,	/* unmute mixer nid 0xc input 1 */
	0x00b37410,	/* unmute mixer nid 0xb beep input and set volume */
};

AZALIA_ARRAY_SIZES;
