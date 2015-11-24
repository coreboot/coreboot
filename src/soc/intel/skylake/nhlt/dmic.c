/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <soc/nhlt.h>
#include <string.h>

static const struct nhlt_format_config dmic_2ch_cfg[] = {
	/* 48 KHz 16-bits per sample. */
	{
		.num_channels = 2,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 16,
		.valid_bits_per_sample = 16,
		.speaker_mask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT,
		.settings_file = "dmic-2ch-48khz-16b.bin",
	},
	/* 48 KHz 32-bits per sample. */
	{
		.num_channels = 2,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 32,
		.valid_bits_per_sample = 32,
		.speaker_mask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT,
		.settings_file = "dmic-2ch-48khz-32b.bin",
	},
};

static const struct nhlt_format_config dmic_4ch_cfg[] = {
	/* 48 KHz 16-bits per sample. */
	{
		.num_channels = 4,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 16,
		.valid_bits_per_sample = 16,
		.speaker_mask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT |
				SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT,
		.settings_file = "dmic-4ch-48khz-16b.bin",
	},
	/* 48 KHz 32-bits per sample. */
	{
		.num_channels = 4,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 32,
		.valid_bits_per_sample = 32,
		.speaker_mask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT |
				SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT,
		.settings_file = "dmic-4ch-48khz-32b.bin",
	},
};

int nhlt_soc_add_dmic_array(struct nhlt *nhlt, int num_channels)
{
	struct nhlt_endpoint *endp;
	struct nhlt_dmic_array_config mic_config;
	const struct nhlt_format_config *formats;
	size_t num_formats;

	if (num_channels != 2 && num_channels != 4)
		return -1;

	endp = nhlt_soc_add_endpoint(nhlt, AUDIO_LINK_DMIC, AUDIO_DEV_DMIC,
					NHLT_DIR_CAPTURE);

	if (endp == NULL)
		return -1;

	memset(&mic_config, 0, sizeof(mic_config));
	mic_config.tdm_config.config_type = NHLT_TDM_MIC_ARRAY;

	switch (num_channels) {
	case 2:
		formats = dmic_2ch_cfg;
		num_formats = ARRAY_SIZE(dmic_2ch_cfg);
		mic_config.array_type = NHLT_MIC_ARRAY_2CH_SMALL;
		break;
	case 4:
		formats = dmic_4ch_cfg;
		num_formats = ARRAY_SIZE(dmic_4ch_cfg);
		mic_config.array_type = NHLT_MIC_ARRAY_4CH_L_SHAPED;
		break;
	}

	if (nhlt_endpoint_append_config(endp, &mic_config, sizeof(mic_config)))
		return -1;

	return nhlt_endpoint_add_formats(endp, formats, num_formats);
}
