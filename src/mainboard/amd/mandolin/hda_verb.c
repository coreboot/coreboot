/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* Realtek ALC701 on mainboard */
	0x10ec0701,
	0x00000000,
	0x00000016,

	AZALIA_SUBVENDOR(0, 0x1022D001), // HDA Codec Subsystem ID: 0x1022D001

	AZALIA_RESET(1), // Widget node 0x01 :
	AZALIA_PIN_CFG(0, 0x12, 0xb7a60140), // Pin widget 0x12 - DMIC
	AZALIA_PIN_CFG(0, 0x13, 0x40000000), // Pin widget 0x13 - DMIC
	AZALIA_PIN_CFG(0, 0x14, 0x90170110), // Pin widget 0x14 - FRONT (Port-D)
	AZALIA_PIN_CFG(0, 0x15, AZALIA_PIN_CFG_NC(0)), // Pin widget 0x15 - I2S-OUT
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)), // Pin widget 0x16 - LINE3 (Port-B)
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(0)), // Pin widget 0x17 - I2S-OUT
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_CFG_NC(0)), // Pin widget 0x18 - I2S-IN
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)), // Pin widget 0x19 - MIC2 (Port-F)
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)), // Pin widget 0x1A - LINE1 (Port-C)
	AZALIA_PIN_CFG(0, 0x1b, 0x04a11050), // Pin widget 0x1B - LINE2 (Port-E)
	AZALIA_PIN_CFG(0, 0x1d, 0x40600001), // Pin widget 0x1D - PC-BEEP
	AZALIA_PIN_CFG(0, 0x1e, 0x04451130), // Pin widget 0x1E - S/PDIF-OUT
	AZALIA_PIN_CFG(0, 0x1f, AZALIA_PIN_CFG_NC(0)), // Pin widget 0x1F - S/PDIF-IN
	AZALIA_PIN_CFG(0, 0x21, 0x04211020), // Pin widget 0x21 - HP-OUT (Port-I)
	AZALIA_PIN_CFG(0, 0x29, AZALIA_PIN_CFG_NC(0)), // Pin widget 0x29 - I2S-IN
	0x02050038, 0x02047901, 0x0205006b, 0x02040260, // NID 0x20 -0 Set Class-D output
							//  power as 2.2W@4 Ohm, and
							//  MIC2-VREFO-R is controlled by
							//  Line2 port.
	0x0205001a, 0x02048c03, 0x02050045, 0x0204b289, // NID 0x20 - 1
	0x0205004a, 0x0204201b, 0x0205004a, 0x0204201b, // NID 0x20 - 2
	0x02050010, 0x02040420, 0x01470c00, 0x02050036, // Dos beep path - 1
	0x02047151, 0x01470740, 0x0143b000, 0x01470c02, // Dos beep path - 2

	/* Realtek ALC285 on extension card */
	0x10ec0285,
	0x00000000,
	0x00000028,

	AZALIA_SUBVENDOR(0, 0x1022D002),

	AZALIA_RESET(1), // Widget node 0x01 :
	AZALIA_PIN_CFG(0, 0x12, 0xb7a60140), // Pin widget 0x12 - DMIC
	AZALIA_PIN_CFG(0, 0x13, 0x40000000), // Pin widget 0x13 - DMIC
	AZALIA_PIN_CFG(0, 0x14, 0x90170110), // Pin widget 0x14 - Front (Port-D)
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)), // Pin widget 0x16 - NPC
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(0)), // Pin widget 0x17 - I2S OUT
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_CFG_NC(0)), // Pin widget 0x18 - I2S IN
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)), // Pin widget 0x19 - MIC2 (Port-F)
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)), // Pin widget 0x1A - NPC
	AZALIA_PIN_CFG(0, 0x1b, 0x04a19030), // Pin widget 0x1B - LINE2 (Port-E)
	AZALIA_PIN_CFG(0, 0x1d, 0x4066192d), // Pin widget 0x1D - BEEP-IN
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)), // Pin widget 0x1E - S/PDIF-OUT
	AZALIA_PIN_CFG(0, 0x21, 0x04211020), // Pin widget 0x21 - HP1-OUT (Port-I)
	0x05c50011, 0x05c40003, 0x05c50011, 0x05c40003, // dis. Silence detect delay turn off
	0x0205003c, 0x0204f254, 0x0205003c, 0x0204f214, // Class-D power on reset
	0x02050045, 0x0204b009, 0x02050063, 0x02040020, // Set TRS + turn off MIC2 VREFO
							//  gating with HP-JD.
	0x0205004a, 0x020420b0, 0x02050009, 0x02043803, // Enable HP JD + Set JD2 to 1 port
							//  JD for WoV
	0x0205000b, 0x0204777a, 0x0205000b, 0x0204777a, // Set TRS + Set JD2 pull up.
	0x02050038, 0x02043909, 0x05c50000, 0x05c43482, // NID 0x20 set class-D to 2W@4ohm
							//  (+12dB gain) + Set sine
							//  tone gain(0x34)
	0x05350000, 0x0534002a, 0x05350000, 0x0534002a, // Disable EQ + set 100Hz 2nd High
							//  Pass filter
	0x0535001d, 0x05340800, 0x0535001e, 0x05340800, // Left Channel-1
	0x05350005, 0x053403f6, 0x05350006, 0x0534854c, // Left Channel-2
	0x05350007, 0x05341e09, 0x05350008, 0x05346472, // Left Channel-3
	0x05350009, 0x053401fb, 0x0535000a, 0x05344836, // Left Channel-4
	0x0535000b, 0x05341c00, 0x0535000c, 0x05340000, // Left Channel-5
	0x0535000d, 0x05340200, 0x0535000e, 0x05340000, // Left Channel-6
	0x05450000, 0x05440000, 0x0545001d, 0x05440800, // Right Channel-1
	0x0545001e, 0x05440800, 0x05450005, 0x054403f6, // Right Channel-2
	0x05450006, 0x0544854c, 0x05450007, 0x05441e09, // Right Channel-3
	0x05450008, 0x05446472, 0x05450009, 0x054401fb, // Right Channel-4
	0x0545000a, 0x05444836, 0x0545000b, 0x05441c00, // Right Channel-5
	0x0545000c, 0x05440000, 0x0545000d, 0x05440200, // Right Channel-6
	0x0545000e, 0x05440000, 0x05350000, 0x0534c02a, // Right Channel-7+ EQ Update & Enable
	0x05d50006, 0x05d44c50, 0x05d50002, 0x05d46004, // AGC-1 Disable + (Front Gain=0dB )
	0x05d50003, 0x05d45e5e, 0x05d50001, 0x05d4d783, // AGC-2 (Back Boost Gain = -0.375dB,
							//  Limiter = -1.125dB)
	0x05d50009, 0x05d451ff, 0x05d50006, 0x05d44e50, // AGC-3 + AGC Enable
	0x02050010, 0x02040020, 0x02050040, 0x02048800, // EAPD set to verb-control.
	0x02050030, 0x02049000, 0x02050037, 0x0204fe15, // Class D silent detection Enable
							//  -84dB threshold
	0x05b50006, 0x05b40044, 0x05a50001, 0x05a4001f, // Set headphone gain and Set pin1
							//  to GPIO2
};

const u32 pc_beep_verbs[] = {
};

AZALIA_ARRAY_SIZES;
