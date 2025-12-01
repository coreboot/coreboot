/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_RAURU_VARIANTS_H__
#define __MAINBOARD_GOOGLE_RAURU_VARIANTS_H__

#include <soc/display.h>

enum audio_amplifier_id {
	AUD_AMP_ID_UNKNOWN,
	AUD_AMP_ID_NAU8318,
	AUD_AMP_ID_TAS2563,
	AUD_AMP_ID_ALC5645,
};

enum audio_amplifier_id get_audio_amp_id(void);

void fw_config_panel_override(struct panel_description *panel);

#endif /* __MAINBOARD_GOOGLE_RAURU_VARIANTS_H__ */
