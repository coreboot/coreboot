/*
 * This file is part of the coreboot project.
 *
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

static const struct nhlt_format_config ssm4567_render_formats[] = {
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
static const struct nhlt_format_config ssm4567_capture_formats[] = {
	/* 48 KHz 32-bits per sample. */
	{
		.num_channels = 4,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 32,
		.valid_bits_per_sample = 32,
		.settings_file = "ssm4567-capture-4ch-48khz-32b.bin",
	},
};

static const struct nhlt_endp_descriptor ssm4567_descriptors[] = {
	/* Render Endpoint */
	{
		.link = NHLT_LINK_SSP,
		.device = NHLT_SSP_DEV_I2S,
		.direction = NHLT_DIR_RENDER,
		.vid = NHLT_VID,
		.did = NHLT_DID_SSP,
		.formats = ssm4567_render_formats,
		.num_formats = ARRAY_SIZE(ssm4567_render_formats),
	},
	/* Capture Endpoint */
	{
		.link = NHLT_LINK_SSP,
		.device = NHLT_SSP_DEV_I2S,
		.direction = NHLT_DIR_CAPTURE,
		.vid = NHLT_VID,
		.did = NHLT_DID_SSP,
		.formats = ssm4567_capture_formats,
		.num_formats = ARRAY_SIZE(ssm4567_capture_formats),
	},
};

int nhlt_soc_add_ssm4567(struct nhlt *nhlt, int hwlink)
{
	/* Virtual bus id of SSP links are the hardware port ids proper. */
	return nhlt_add_ssp_endpoints(nhlt, hwlink, ssm4567_descriptors,
					ARRAY_SIZE(ssm4567_descriptors));
}
