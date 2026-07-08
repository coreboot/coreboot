/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 via_vt1708s_verbs[] = {
	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x11060000 */
	AZALIA_SUBVENDOR(0, 0x11060000),

	/* Pin Widget Verb Table */

	/*
	 * NID 0x19 [Port A (SURR)]:
	 * Jack     Internal    Speaker     N/A             Black
	 */
	AZALIA_PIN_CFG(0, 0x19, 0x10101112),

	/*
	 * NID 0x1a [Port B (MIC1/2)]:
	 * Jack     Rear        Mic In      1/8"            Pink
	 */
	AZALIA_PIN_CFG(0, 0x1a, 0x01a19036),

	/*
	 * NID 0x1b [Port C (LINEIN)]:
	 * Jack     Rear        Line In     1/8"            Blue
	 */
	AZALIA_PIN_CFG(0, 0x1b, 0x0181303e),

	/*
	 * NID 0x1c [Port D (Front)]:
	 * Jack     Rear        Line Out    1/8"            Green
	 */
	AZALIA_PIN_CFG(0, 0x1c, 0x01014010),

	/*
	 * NID 0x1d [Port E (Front HP/MIC)]:
	 * Jack     Front       HP Out      1/8"            Green
	 */
	AZALIA_PIN_CFG(0, 0x1d, 0x022141f0),

	/*
	 * NID 0x1e [Port F (Front HP/MIC)]:
	 * Jack     Front       Mic In      1/8"            Pink
	 */
	AZALIA_PIN_CFG(0, 0x1e, 0x02a19138),

	/*
	 * NID 0x1f [CD]:
	 * Jack     Int.(ATAPI) CD          ATAPI internal  Black
	 */
	AZALIA_PIN_CFG(0, 0x1f, 0x19331137),

	/*
	 * NID 0x20 [N/A]:
	 * Jack     Rear        S/PDIF Out  RCA             Unknown
	 */
	AZALIA_PIN_CFG(0, 0x20, 0x014401f0),

	/*
	 * NID 0x21 [N/A]:
	 * None     Internal    S/PDIF Out  Other Digital   Unknown
	 */
	AZALIA_PIN_CFG(0, 0x21, 0x504600f0),

	/*
	 * NID 0x22 [Port G (C/LFE)]:
	 * Jack     Internal    Speaker     Unknown         Orange
	 */
	AZALIA_PIN_CFG(0, 0x22, 0x10106111),

	/*
	 * NID 0x23 [Port H (SSL/SSR)]:
	 * Jack     Internal    Speaker     Unknown         Grey
	 */
	AZALIA_PIN_CFG(0, 0x23, 0x10102114),
};

static const u32 intel_pantherpoint_hdmi_verbs[] = {
	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x80860101 */
	AZALIA_SUBVENDOR(3, 0x80860101),

	/* Pin Complex (NID 0x05) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(3, 0x05, 0x58560010),

	/* Pin Complex (NID 0x06) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(3, 0x06, 0x18560020),

	/* Pin Complex (NID 0x07) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[0] = {};

static struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Via VT1708S",
		.vendor_id    = 0x11060397,
		.subsystem_id = 0x11060000,
		.address      = 0,
		.verbs        = via_vt1708s_verbs,
		.verb_count   = ARRAY_SIZE(via_vt1708s_verbs),
	},
	{
		.name         = "Intel PantherPoint HDMI",
		.vendor_id    = 0x80862806,
		.subsystem_id = 0x80860101,
		.address      = 3,
		.verbs        = intel_pantherpoint_hdmi_verbs,
		.verb_count   = ARRAY_SIZE(intel_pantherpoint_hdmi_verbs),
	},
};

AZALIA_ARRAY_SIZES;
