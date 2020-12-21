/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/cbi_ssfc.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>

static int get_ssfc(uint32_t *val)
{
	static uint32_t known_value;
	static enum {
		SSFC_NOT_READ,
		SSFC_AVAILABLE,
	} ssfc_state = SSFC_NOT_READ;

	if (ssfc_state == SSFC_AVAILABLE) {
		*val = known_value;
		return 0;
	}

	/*
	 * If SSFC field is not in the CBI then the value of SSFC will be 0 for
	 * further processing later since 0 of each bits group means default
	 * component in a variant. For more detail, please refer to cbi_ssfc.h.
	 */
	if (google_chromeec_cbi_get_ssfc(&known_value) != 0) {
		printk(BIOS_DEBUG, "SSFC not set in CBI\n");
		return -1;
	}

	ssfc_state = SSFC_AVAILABLE;
	*val = known_value;
	printk(BIOS_INFO, "SSFC 0x%x.\n", known_value);

	return 0;
}

static unsigned int extract_field(uint32_t mask, int shift)
{
	uint32_t ssfc;

	/* On errors nothing is assumed to be set. */
	if (get_ssfc(&ssfc))
		return 0;

	return (ssfc >> shift) & mask;
}

static enum ssfc_audio_codec ssfc_get_default_audio_codec(void)
{
	/*
	 * Octopus has two reference boards; yorp is with DA7219 and bip is with
	 * RT5682. Currently only AMPTON derived from bip so only it uses
	 * RT5682 as the default source in the first MP devices.
	 */
	if (CONFIG(BOARD_GOOGLE_AMPTON))
		return SSFC_AUDIO_CODEC_RT5682;

	return SSFC_AUDIO_CODEC_DA7219;
}

enum ssfc_audio_codec ssfc_get_audio_codec(void)
{
	uint32_t codec = extract_field(
			SSFC_AUDIO_CODEC_MASK, SSFC_AUDIO_CODEC_OFFSET);

	if (codec != SSFC_AUDIO_CODEC_DEFAULT)
		return codec;

	return ssfc_get_default_audio_codec();
}
