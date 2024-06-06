/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0283,	// Codec Vendor / Device ID: Realtek ALC283
	0x10ec0283,	// Subsystem ID
	0x00000013,	// Number of jacks (NID entries)

	0x0017ff00,	// Function Reset
	0x0017ff00,	// Double Function Reset
	0x000F0000,	// Pad - get vendor id
	0x000F0002,	// Pad - get revision id

	/* Bits 31:28 - Codec Address */
	/* Bits 27:20 - NID */
	/* Bits 19:8 - Verb ID */
	/* Bits 7:0  - Payload */

	/* NID 0x01, HDA Codec Subsystem ID Verb Table */
	AZALIA_SUBVENDOR(0, 0x11790670),

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x12) DMIC - Enabled */
	AZALIA_PIN_CFG(0, 0x12, 0x90a60130),

	/* Pin Complex (NID 0x14) SPKR-OUT - Internal Speakers */
	// group 1, cap 0
	// no connector, no jack detect
	// speaker out, analog
	// fixed function, internal, Location N/A
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),

	/* Pin Complex (NID 0x17) MONO Out - Enabled */
	AZALIA_PIN_CFG(0, 0x17, 0x40000008),

	/* Pin Complex (NID 0x18) Disabled */
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_CFG_NC(0)),

	/* Pin Complex (NID 0x19)  MIC2 - 3.5mm Jack */
	// group2, cap 0
	// black, jack detect
	// Mic in, 3.5mm Jack
	// connector, External left panel
	AZALIA_PIN_CFG(0, 0x19, 0x03a11020),

	/* Pin Complex (NID 0x1A)  LINE1 - Disabled */
	AZALIA_PIN_CFG(0, 0x1A, AZALIA_PIN_CFG_NC(0)),

	/* Pin Complex (NID 0x1B)  LINE2 - Disabled */
	AZALIA_PIN_CFG(0, 0x1B, AZALIA_PIN_CFG_NC(0)),

	/* Pin Complex (NID 0x1D)  PCBeep */
	// eapd low on ex-amp, laptop, custom enable
	// mute spkr on hpout
	// pcbeep enable, checksum
	// no physical, Internal, Location N/A
	AZALIA_PIN_CFG(0, 0x1D, 0x4015812d),

	/* Pin Complex (NID 0x1E)  SPDIF-OUT - Disabled*/
	AZALIA_PIN_CFG(0, 0x1E, AZALIA_PIN_CFG_NC(0)),

	/* Pin Complex (NID 0x21) HP-OUT - 3.5mm Jack*/
	// group1
	// black, jack detect
	// HPOut, 3.5mm Jack
	// connector, left panel
	AZALIA_PIN_CFG(0, 0x21, 0x0321101f),

	/* Undocumented settings from Realtek (needed for beep_gen) */
	/* Widget node 0x20 */
	0x02050038,
	0x02046900,
	0x02050010,
	0x02040C20,
	/* Widget node 0x20 - 1 */
	0x02050019,
	0x02041857,
	0x0205001A,
	0x02044001,
	/* Widget node 0x20 - 2 */
	0x0205001B,
	0x0204140B,
	0x02050025,
	0x0204802A,
	/* Widget node 0x20 - 3 */
	0x02050045,
	0x02045029,
	0x02050046,
	0x02040004,
	/* Widget node 0x20 - 4 */
	0x02050043,
	0x0204A614,
	0x02050043,
	0x0204A614,
	/* pc beep */
	0x02050010,
	0x02040c20,
	0x0205001b,
	0x0204081b,

	/* Tuned jack detection */
	0x02050043,
	0x0204A614,
	0x02050047,
	0x02049470,
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
