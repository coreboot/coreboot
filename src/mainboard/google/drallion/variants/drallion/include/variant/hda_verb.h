/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_HDA_VERB_H
#define MAINBOARD_HDA_VERB_H

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0295,	// Codec Vendor / Device ID: Realtek ALC3254
	0xffffffff,	// Subsystem ID
	0x0000002b,	// Number of jacks (NID entries)

	/* Rest Codec First */
	AZALIA_RESET(0x1),
	/* NID 0x01, HDA Codec Subsystem ID Verb Table */
	AZALIA_SUBVENDOR(0, 0x102808b6),

	/* Pin Widget Verb Table */
	AZALIA_PIN_CFG(0, 0x12, 0xb7a60130),
	AZALIA_PIN_CFG(0, 0x13, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x14, 0x90170110),
	AZALIA_PIN_CFG(0, 0x16, 0x40000000),
	AZALIA_PIN_CFG(0, 0x17, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x18, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x19, 0x04a11030),
	AZALIA_PIN_CFG(0, 0x1a, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1b, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x40c00001),
	AZALIA_PIN_CFG(0, 0x1e, 0x421212f2),
	AZALIA_PIN_CFG(0, 0x21, 0x04211020),

	/* D reset */
	0x0205003C,
	0x0204F254,
	0x0205003C,
	0x0204F214,
	/* JD1 - 2port JD mode */
	0x02050009,
	0x0204E003,
	0x0205000A,
	0x02047770,
	/* Set TRS type-1 */
	0x02050045,
	0x02045289,
	0x02050049,
	0x02040049,
	/* Set TRS type-2 + Set UAJ Line2 vref(ALC3254) */
	0x0205004A,
	0x0204A830,
	0x02050063,
	0x0204CF00,
	/* NID 0x20 set class-D to 2W@4ohm (+12dB gain)
	 * + Set sine tone gain(0x34) */
	0x02050038,
	0x02043909,
	0x05C50000,
	0x05C43482,
	/* AGC-1 Disable + (Front Gain=0dB ) */
	0x05D50006,
	0x05D44C50,
	0x05D50002,
	0x05D44004,
	/* AGC-2   (Backt Boost Gain= -0.375dB  ,Limiter = -3dB) */
	0x05D50003,
	0x05D45E5E,
	0x05D50001,
	0x05D4D788,
	/* AGC-3 + AGC Enable */
	0x05D50009,
	0x05D451FF,
	0x05D50006,
	0x05D44E50,
	/* HP-JD Enable +Nokia type */
	0x0205004A,
	0x02042010,
	0x02050008,
	0x02046A0C,
	/* EAPD set to verb-control + I2C Un-use+ DVDD3.3V */
	0x02050010,
	0x02040020,
	0x02050034,
	0x0204A23D,
	/* Class D silent detection Enable -84dB threshold */
	0x02050030,
	0x02049000,
	0x02050037,
	0x0204FE15,
	/* Disable EQ + set 250Hz 3rd High Pass filter */
	0x05350000,
	0x0534203A,
	0x05350000,
	0x0534203A,
	/* Left Channel-1 */
	0x0535001d,
	0x05340800,
	0x0535001e,
	0x05340800,
	/* Left Channel-2 */
	0x05350003,
	0x05341EF8,
	0x05350004,
	0x05340000,
	/* Left Channel-3 */
	0x05350005,
	0x053403EE,
	0x05350006,
	0x0534FA60,
	/* Left Channel-4 */
	0x05350007,
	0x05341E10,
	0x05350008,
	0x05347B86,
	/* Left Channel-5 */
	0x05350009,
	0x053401F7,
	0x0535000A,
	0x05349FB6,
	/* Left Channel-6 */
	0x0535000B,
	0x05341C00,
	0x0535000C,
	0x05340000,
	/* Left Channel-7 */
	0x0535000D,
	0x05340200,
	0x0535000E,
	0x05340000,
	/* Right Channel-1 */
	0x05450000,
	0x05442000,
	0x0545001d,
	0x05440800,
	/* Right Channel-2 */
	0x0545001e,
	0x05440800,
	0x05450003,
	0x05441EF8,
	/* Right Channel-3 */
	0x05450004,
	0x05440000,
	0x05450005,
	0x054403EE,
	/* Right Channel-4 */
	0x05450006,
	0x0544FA60,
	0x05450007,
	0x05441E10,
	/* Right Channel-5 */
	0x05450008,
	0x05447B86,
	0x05450009,
	0x054401F7,
	/* Right Channel-6 */
	0x0545000A,
	0x05449FB6,
	0x0545000B,
	0x05441C00,
	/* Right Channel-7 */
	0x0545000C,
	0x05440000,
	0x0545000D,
	0x05440200,
	/* Right Channel-8 + EQ Update & Enable */
	0x0545000E,
	0x05440000,
	0x05350000,
	0x0534E03A,
	/* Enable all Microphone */
	0x0205000D,
	0x0204A023,
	0x0205000D,
	0x0204A023,
	/* Enable Internal Speaker (NID14) */
	0x0205000F,
	0x02040000,
	0x0205000F,
	0x02040000,

	0x8086280b,	/* Codec Vendor/Device ID: Intel CometPoint HDMI */
	0x80860101,	/* Subsystem ID */
	4,		/* Number of entries */

	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x07, 0x18560010),
};

const u32 pc_beep_verbs[] = {
	/* PCBeep pass through to NID14 for ePSA test-1 */
	0x02050036,
	0x020477D7,
	0x0143B000,
	0x01470740,
	/* PCBeep pass through to NID14 for ePSA test-2 */
	0x01470C02,
	0x01470C02,
	0x01470C02,
	0x01470C02,
};

AZALIA_ARRAY_SIZES;

#endif
