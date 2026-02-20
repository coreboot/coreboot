/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>

static const u32 realtek_alc285_verbs[] = {
	AZALIA_SUBVENDOR(0, 0xf111000c),
	AZALIA_RESET(1),

	/* Pin widget configurations */
	/* DMIC - Internal Mic */
	AZALIA_PIN_CFG(0, 0x12, AZALIA_PIN_DESC(
		AZALIA_INTEGRATED,
		AZALIA_INTERNAL,
		AZALIA_MIC_IN,
		AZALIA_OTHER_DIGITAL,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_NO_JACK_PRESENCE_DETECT,
		3, 0
	)),
	AZALIA_PIN_CFG(0, 0x13, 0x40000000), /* DMIC - NC */
	/* Front (Port-D) - Internal Speaker */
	AZALIA_PIN_CFG(0, 0x14, AZALIA_PIN_DESC(
		AZALIA_INTEGRATED,
		AZALIA_INTERNAL,
		AZALIA_SPEAKER,
		AZALIA_OTHER_ANALOG,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_NO_JACK_PRESENCE_DETECT,
		1, 0
	)),
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)), /* NPC */
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(0)), /* I2S OUT */
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_CFG_NC(0)), /* I2S IN */
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)), /* MIC2 (Port-F) */
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)), /* NPC */
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)), /* LINE2 (Port-E) */
	AZALIA_PIN_CFG(0, 0x1d, 0x40651b05), /* BEEP-IN */
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)), /* S/PDIF-OUT */
	/* HP1-OUT (Port-I) - Headphone Jack */
	AZALIA_PIN_CFG(0, 0x21, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_RIGHT,
		AZALIA_HP_OUT,
		AZALIA_STEREO_MONO_1_8,
		AZALIA_BLACK,
		AZALIA_JACK_PRESENCE_DETECT,
		2, 0
	)),

	/* Reset power state to D0 */
	0x00170500, 0x00170500, 0x00170500, 0x00170500,

	/* Reset all registers back to default */
	0x0205001a, 0x02048003, 0x0205001a, 0x0204c003,

	/* ClassD reset */
	0x0205003c, 0x0204f2d4, 0x0205003c, 0x0204f214,

	/* JD1 - 2port JD mode */
	0x02050009, 0x0204e003, 0x0205000a, 0x02047770,

	/* Set TRS type-1 */
	0x02050045, 0x0204c689, 0x02050049, 0x02040049,

	/* Set TRS type-2 (Turn off MIC2_Vrefo_R_L) + Set UAJ Line2 vref */
	0x0205004a, 0x0204a830, 0x02050063, 0x02040f00,

	/* NID 0x20 set class-D to 2W@4ohm (+12dB gain) + Set sine tone gain(0x34) */
	0x02050038, 0x02047909, 0x05c50000, 0x05c43482,

	/* HP-JD Enable + For Nokia type (+power down JD2) */
	0x0205004a, 0x02042010, 0x02050008, 0x02046a2c,

	/* EAPD set to verb-control (I2C unuse & DVDD= +3.3V) */
	0x02050010, 0x02040020, 0x02050034, 0x0204a2b1,

	/* Class D silent detection enable -84dB threshold */
	0x02050030, 0x02049000, 0x02050037, 0x0204fe15,

	/* PCBeep pass through to NID14 for ePSA test-1 */
	0x02050036, 0x020477d7, 0x0143b000, 0x01470740,

	/* PCBeep pass through to NID14 for ePSA test-2 */
	0x01470c02, 0x01470c02, 0x01470c02, 0x01470c02,

	/* ALC285 EQ Verb table - HPF 150Hz - disable EQ first */
	0x05350000, 0x0534201a, 0x05350000, 0x0534201a,
	0x0535001d, 0x05340800, 0x0535001e, 0x05340800,
	0x05350003, 0x05341f61, 0x05350004, 0x05340000,
	0x05450000, 0x05442000, 0x0545001d, 0x05440800,
	0x0545001e, 0x05440800, 0x05450003, 0x05441f61,
	0x05450004, 0x05440000, 0x05350000, 0x0534e01a,
	0x02050038, 0x02047909, 0x05350002, 0x05348000,
	0x05d50006, 0x05d44c50,
};

static const u32 intel_adl_hdmi_verbs[] = {
	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x04, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x08, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0a, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0b, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0c, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0d, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0e, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0f, 0x18560010),
};

const u32 pc_beep_verbs[] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC285",
		.vendor_id    = 0x10ec0285,
		.subsystem_id = 0xf111000c,
		.address      = 0,
		.verbs        = realtek_alc285_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc285_verbs),
	},
	{
		.name         = "Intel Alderlake HDMI",
		.vendor_id    = 0x8086281c,
		.subsystem_id = 0x80860101,
		.address      = 2,
		.verbs        = intel_adl_hdmi_verbs,
		.verb_count   = ARRAY_SIZE(intel_adl_hdmi_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
