/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/helpers.h>
#include <soc/nhlt.h>

/*
 * The same DSP firmware settings are used for both the capture and
 * render endpoints.
*/
static const struct nhlt_format_config rt5663_formats[] = {
	/* 48 KHz 24-bits per sample. */
	{
		.num_channels = 2,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 32,
		.valid_bits_per_sample = 24,
		.speaker_mask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT,
		.settings_file = "rt5663-2ch-48khz-24b.bin",
	},
};

/*
 * The rt5663 just has headphones and a mic. Both the capture and render
 * endpoints occupy the same virtual slot.
*/
static const struct nhlt_tdm_config tdm_config = {
	.virtual_slot = 0,
	.config_type = NHLT_TDM_BASIC,
};

static const struct nhlt_endp_descriptor rt5663_descriptors[] = {
	/* Render Endpoint */
	{
		.link = NHLT_LINK_SSP,
		.device = NHLT_SSP_DEV_I2S,
		.direction = NHLT_DIR_RENDER,
		.vid = NHLT_VID,
		.did = NHLT_DID_SSP,
		.cfg = &tdm_config,
		.cfg_size = sizeof(tdm_config),
		.formats = rt5663_formats,
		.num_formats = ARRAY_SIZE(rt5663_formats),
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
		.formats = rt5663_formats,
		.num_formats = ARRAY_SIZE(rt5663_formats),
	},
};

int nhlt_soc_add_rt5663(struct nhlt *nhlt, int hwlink)
{
	/* Virtual bus id of SSP links are the hardware port ids proper. */
	return nhlt_add_ssp_endpoints(nhlt, hwlink, rt5663_descriptors,
					ARRAY_SIZE(rt5663_descriptors));
}
