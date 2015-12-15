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

static const struct nhlt_format_config ssm4567_render_cfg[] = {
	/* 48 KHz 24-bits per sample. */
	{
		.num_channels = 2,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 32,
		.valid_bits_per_sample = 24,
		.speaker_mask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT,
		.settings_file = "ssm4567-render-2ch-48khz-24b.bin",
	},
};

/* Capture Blob used for IV feedback for Speaker Protection Algorithm */
static const struct nhlt_format_config ssm4567_capture_cfg[] = {
        /* 48 KHz 32-bits per sample. */
        {
                .num_channels = 4,
                .sample_freq_khz = 48,
                .container_bits_per_sample = 32,
                .valid_bits_per_sample = 32,
                .settings_file = "ssm4567-capture-4ch-48khz-32b.bin",
        },
};

int nhlt_soc_add_ssm4567(struct nhlt *nhlt, int hwlink)
{
	struct nhlt_endpoint *endp;

	/* Render Endpoint */
	endp = nhlt_soc_add_endpoint(nhlt, hwlink, AUDIO_DEV_I2S,
					NHLT_DIR_RENDER);

	if (endp == NULL)
		return -1;

	if (nhlt_endpoint_add_formats(endp, ssm4567_render_cfg,
					ARRAY_SIZE(ssm4567_render_cfg)))
		return -1;

	/* Capture Endpoint for IV Feedback */
	endp = nhlt_soc_add_endpoint(nhlt, hwlink, AUDIO_DEV_I2S,
					NHLT_DIR_CAPTURE);

	if (endp == NULL)
		return -1;

	if (nhlt_endpoint_add_formats(endp, ssm4567_capture_cfg,
					ARRAY_SIZE(ssm4567_capture_cfg)))
		return -1;

	nhlt_next_instance(nhlt, NHLT_LINK_SSP);

	return 0;
}
