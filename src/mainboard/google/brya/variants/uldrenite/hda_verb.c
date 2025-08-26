/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0236,	// Codec Vendor ID: Realtek ALC3204
	0x10280D76,	// Subsystem ID
	0x00000019,	// Number of jacks (NID entries)

	AZALIA_RESET(0x1),
	/* NID 0x01, HDA Codec Subsystem ID Verb table */
	AZALIA_SUBVENDOR(0, 0x10280D76),

	/* Pin Widget Verb Table */
	/*
	 * DMIC
	 * Requirement is to use PCH DMIC. Hence,
	 * commented out codec's Internal DMIC.
	 * AZALIA_PIN_CFG(0, 0x12, 0x90A60130),
	 * AZALIA_PIN_CFG(0, 0x13, 0x40000000),
	 */
	AZALIA_PIN_CFG(0, 0x12, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x13, 0x40000000),
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),
	AZALIA_PIN_CFG(0, 0x18, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x19, 0x00a11030),
	AZALIA_PIN_CFG(0, 0x1a, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1b, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x40700001),
	AZALIA_PIN_CFG(0, 0x1e, 0x421212f2),
	AZALIA_PIN_CFG(0, 0x21, 0x00211020),

	/* RESET to D0 */
	0x00170500,
	0x00170500,
	0x00170500,
	0x00170500,
	/* RESET Register */
	0x0205001A,
	0x02048003,
	0x0205001A,
	0x0204C003,
	/* ALC3204 default-1(Class D RESET) */
	0x0205003C,
	0x02040354,
	0x0205003C,
	0x02040314,
	/* ALC3204 default-2 (Tco) */
	0x02050040,
	0x02049800,
	0x02050034,
	0x0204023C,
	/* ALC3204 Speaker output power - 4 ohm
	 * 2W + Combo Jack default setting
	 */
	0x02050038,
	0x02047901,
	0x02050045,
	0x02045089,
	/* Zero data  + EAPD to verb-control */
	0x02050037,
	0x0204FE15,
	0x02050010,
	0x02040020,
	/* Zero data */
	0x02050030,
	0x02048000,
	0x02050030,
	0x02048000,
	/* ALC3204 default-3 */
	0x05750003,
	0x05740DA3,
	0x02050046,
	0x02040004,
	/* ALC3204 default-4 */
	0x0205001B,
	0x02040A4B,
	0x02050008,
	0x02046A4C,
	/* JD1 */
	0x02050009,
	0x0204E003,
	0x0205000A,
	0x02047770,
	/* JD2 */
	0x0205000B,
	0x02047778,
	0x0205000B,
	0x02047778,
	/* Microphone + Array MIC  security Disable  +ADC clock Enable */
	0x0205000D,
	0x0204A020,
	0x02050005,
	0x02040700,
	/* Speaker Enable */
	0x0205000C,
	0x020401EF,
	0x0205000C,
	0x020401EF,
};

const u32 pc_beep_verbs[] = {
	/* PCBeep pass through to NID14 for ePSA test-1 */
	0x02050036,
	0x02047717,
	0x02050036,
	0x02047717,
	/* PCBeep pass through to NID14 for ePSA test-2 */
	0x01470740,
	0x0143B000,
	0x01470C02,
	0x01470C02,
};

AZALIA_ARRAY_SIZES;
