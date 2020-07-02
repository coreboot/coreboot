/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __EC_GOOGLE_CHROMEEC_AUDIO_CODEC__
#define __EC_GOOGLE_CHROMEEC_AUDIO_CODEC__

struct ec_google_chromeec_audio_codec_config {
	/* ACPI device name */
	const char *name;
	/* ACPI _UID */
	unsigned int uid;
};

#endif /* __EC_GOOGLE_CHROMEEC_AUDIO_CODEC__ */
