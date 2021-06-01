/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _HDA_VERB_H_
#define _HDA_VERB_H_

#include <device/azalia_device.h>
#include <device/azalia.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0256,	// Codec Vendor / Device ID: Realtek ALC256
	0xffffffff,	// Subsystem ID
	0x0000002b,	// Number of jacks (NID entries)

	/* Rest Codec First */
	AZALIA_RESET(0x1),

	/* HDA Codec Subsystem ID Verb-table
	 HDA Codec Subsystem ID: 0x10EC119E */
	0x0017209E,
	0x00172111,
	0x001722EC,
	0x00172310,
	/* Pin Widget Verb-table */
	AZALIA_PIN_CFG(0, 0x01, 0x00000000),
	AZALIA_PIN_CFG(0, 0x12, 0x90a61120),
	AZALIA_PIN_CFG(0, 0x13, 0x40000000),
	AZALIA_PIN_CFG(0, 0x14, 0x90171110),
	AZALIA_PIN_CFG(0, 0x18, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x19, 0x02ab1020),
	AZALIA_PIN_CFG(0, 0x1a, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1b, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1e, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x21, 0x022b1010),
	/* ONE DOES NOT SIMPLY
	   MAKE IT WORK WITH WINDOWS */
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
	/* ALC256 default-1(Class D RESET) */
	0x0205003C,
	0x02040354,
	0x0205003C,
	0x02040314,
	/* ALC256 default-2 */
	0x02050040,
	0x02049800,
	0x02050034,
	0x0204023C,
	/* ALC256 Speaker output power - 4 ohm 2.2W (+12dB gain) + Combo Jack TRS setting */
	0x02050038,
	0x02047901,
	0x02050045,
	0x02045089,
	/* H/W AGC setting-1 */
	0x02050016,
	0x02040C50,
	0x02050012,
	0x0204EBC2,
	/* H/W AGC setting-2 */
	0x02050013,
	0x0204401D,
	0x02050016,
	0x02044E50,
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
	/* ALC256 default-3 */
	0x05750003,
	0x05740DA3,
	0x02050046,
	0x02040004,
	/* ALC256 default-4 */
	0x0205001B,
	0x02040A4B,
	0x02050008,
	0x02046A6C,
	/* JD1 */
	0x02050009,
	0x0204E003,
	0x0205000A,
	0x02047770,
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
	/* EQ Bypass + EQ HPF cutoff 250Hz */
	0x05350000,
	0x0534201A,
	0x0535001d,
	0x05340800,
	/* EQ-2 */
	0x0535001e,
	0x05340800,
	0x05350003,
	0x05341EF8,
	/* EQ-3 */
	0x05350004,
	0x05340000,
	0x05450000,
	0x05442000,
	/* EQ-4 */
	0x0545001d,
	0x05440800,
	0x0545001e,
	0x05440800,
	/* EQ-5 */
	0x05450003,
	0x05441EF8,
	0x05450004,
	0x05440000,
	/* EQ Update */
	0x05350000,
	0x0534E01A,
	0x05350000,
	0x0534E01A,

	0x8086280b, /* Codec Vendor / Device ID: Intel */
	0x80860101, /* Subsystem ID */
	0x00000004, /* Number of 4 dword sets */

	AZALIA_SUBVENDOR(2, 0x80860101),

	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x07, 0x18560010),
};

const u32 pc_beep_verbs[] = {
};

AZALIA_ARRAY_SIZES;

#endif
