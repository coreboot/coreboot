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

/* The same DSP firmwware settings are used for both the capture and
 * render endpoints. */
static const struct nhlt_format_config nau88l25_cfg[] = {
	/* 48 KHz 24-bits per sample. */
	{
		.num_channels = 2,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 32,
		.valid_bits_per_sample = 24,
		.speaker_mask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT,
		.settings_file = "nau88l25-2ch-48khz-24b.bin",
	},
};

int nhlt_soc_add_nau88l25(struct nhlt *nhlt, int hwlink)
{
	struct nhlt_endpoint *endp;
	/* The nau88l25 just has headphones and a mic. Both the capture and
	 * render endpoints occupy the same virtual slot. */
	struct nhlt_tdm_config tdm_config = {
		.virtual_slot = 0,
		.config_type = NHLT_TDM_BASIC,
	};
	const void *fmt_cfg = nau88l25_cfg;
	size_t fmt_sz = ARRAY_SIZE(nau88l25_cfg);

	/* Render Endpoint */
	endp = nhlt_soc_add_endpoint(nhlt, hwlink, AUDIO_DEV_I2S,
					NHLT_DIR_RENDER);

	if (endp == NULL)
		return -1;

	if (nhlt_endpoint_append_config(endp, &tdm_config, sizeof(tdm_config)))
		return -1;

	if (nhlt_endpoint_add_formats(endp, fmt_cfg, fmt_sz))
		return -1;

	/* Capture Endpoint */
	endp = nhlt_soc_add_endpoint(nhlt, hwlink, AUDIO_DEV_I2S,
					NHLT_DIR_CAPTURE);

	if (endp == NULL)
		return -1;

	if (nhlt_endpoint_append_config(endp, &tdm_config, sizeof(tdm_config)))
		return -1;

	if (nhlt_endpoint_add_formats(endp, fmt_cfg, fmt_sz))
		return -1;

	nhlt_next_instance(nhlt, NHLT_LINK_SSP);

	return 0;
}
