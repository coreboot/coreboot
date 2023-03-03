/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>
#include <ec/purism/librem-ec/librem_ec.h>
#include <console/console.h>

const u32 cim_verb_data[] = {
	0x10ec0256,	/* Codec Vendor/Device ID: Realtek ALC256 */
	0x10ec0256,	/* Subsystem ID */
	16,		/* Number of entries */

	AZALIA_RESET(0x1),

	AZALIA_SUBVENDOR(0, 0x10ec0256),
	AZALIA_PIN_CFG(0, 0x12, 0x90a60140), /* Front digital mic */
	AZALIA_PIN_CFG(0, 0x13, 0x411111f0), /* NC */
	AZALIA_PIN_CFG(0, 0x14, 0x90170110), /* Internal speakers */
	AZALIA_PIN_CFG(0, 0x18, 0x411111f0), /* NC */
	AZALIA_PIN_CFG(0, 0x19, 0x02a11030), /* Jack analog mic */
	AZALIA_PIN_CFG(0, 0x1a, 0x411111f0), /* NC */
	AZALIA_PIN_CFG(0, 0x1b, 0x411111f0), /* NC */
	AZALIA_PIN_CFG(0, 0x1d, 0x411111f0), /* NC */
	AZALIA_PIN_CFG(0, 0x1e, 0x411111f0), /* NC */
	AZALIA_PIN_CFG(0, 0x21, 0x02211020), /* Jack analog out */

	/* Hidden SW reset */
	0x0205001a,
	0x0204c003,
	0x0205001a,
	0x02048003,

	/* Class D power on reset control */
	0x0205003c,
	0x02040354,
	0x0205003c,
	0x02040314,

	/* LDO3 output set to 1.2V */
	0x0205001b,
	0x02040a4b,
	0x02050019,
	0x02040e12,

	/* SPK power */
	0x02050038,
	0x02046901,
	0x02050007,
	0x02040200,

	0x8086280b,	/* Codec Vendor/Device ID: Intel CometPoint HDMI */
	0x80860101,	/* Subsystem ID */
	4,		/* Number of entries */

	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x07, 0x18560010),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;

/* Older verbs with no jack detect - needed if an older Librem EC is in use that
   lacks jack detect.  Headphones can be selected manually. */
static const u32 no_jack_detect_verbs[] = {
	AZALIA_PIN_CFG(0, 0x19, 0x04a11130), /* Jack analog mic */
	AZALIA_PIN_CFG(0, 0x21, 0x04211120), /* Jack analog out */
};

void mainboard_azalia_program_runtime_verbs(u8 *base, u32 viddid)
{
	if (viddid == 0x10ec0256) {
		/* Now that the codec is configured, we can check if the EC has
		   jack detect. */
		if (librem_ec_has_jack_detect()) {
			printk(BIOS_INFO, "EC jack detect enabled\n");
		} else {
			printk(BIOS_WARNING, "EC firmware lacks jack detect, applying workaround.\n");
			printk(BIOS_WARNING, "Update to Librem-EC 1.13 or later for jack detect.\n");
			/* The EC firmware lacks jack detect.  Program the
			   older workaround verbs with no jack detect. */
			azalia_program_verb_table(base, no_jack_detect_verbs,
				ARRAY_SIZE(no_jack_detect_verbs));
		}
	}
}
