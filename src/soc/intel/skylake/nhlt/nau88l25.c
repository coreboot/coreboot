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

/* The same DSP firmware settings are used for both the capture and
 * render endpoints. */
static const struct nhlt_format_config nau88l25_formats[] = {
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

/* The nau88l25 just has headphones and a mic. Both the capture and render
 * endpoints occupy the same virtual slot. */
static const struct nhlt_tdm_config tdm_config = {
	.virtual_slot = 0,
	.config_type = NHLT_TDM_BASIC,
};

static const struct nhlt_endp_descriptor nau88l25_descriptors[] = {
	/* Render Endpoint */
	{
		.link = NHLT_LINK_SSP,
		.device = NHLT_SSP_DEV_I2S,
		.direction = NHLT_DIR_RENDER,
		.vid = NHLT_VID,
		.did = NHLT_DID_SSP,
		.cfg = &tdm_config,
		.cfg_size = sizeof(tdm_config),
		.formats = nau88l25_formats,
		.num_formats = ARRAY_SIZE(nau88l25_formats),
	},
	/* Capture Endpoint */
	{
		.link = NHLT_LINK_SSP,
		.device = NHLT_SSP_DEV_I2S,
		.direction = NHLT_DIR_CAPTURE,
		.vid = NHLT_VID,
		.did = NHLT_DID_SSP,
		.cfg = &tdm_config,
		.cfg_size = sizeof(tdm_config),
		.formats = nau88l25_formats,
		.num_formats = ARRAY_SIZE(nau88l25_formats),
	},
};

int nhlt_soc_add_nau88l25(struct nhlt *nhlt, int hwlink)
{
	/* Virtual bus id of SSP links are the hardware port ids proper. */
	return nhlt_add_ssp_endpoints(nhlt, hwlink, nau88l25_descriptors,
					ARRAY_SIZE(nau88l25_descriptors));
}
