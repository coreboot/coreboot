/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/helpers.h>
#include <soc/nhlt.h>

static const struct nhlt_format_config max98373_render_formats[] = {
	/* 48 KHz 16-bits per sample. */
	{
		.num_channels = 2,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 16,
		.valid_bits_per_sample = 16,
		.speaker_mask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT,
		.settings_file = "max98373-render-2ch-48khz-16b.bin",
	}
};

static const struct nhlt_format_config max98373_capture_formats[] = {
	/* 48 KHz 16-bits per sample - Quad Channel. */
	{
		.num_channels = 4,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 32,
		.valid_bits_per_sample = 16,
		.speaker_mask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT,
		.settings_file = "max98373-render-2ch-48khz-16b.bin",
	},
	/* 48 KHz 16-bits per sample - Stereo Channel */
	{
		.num_channels = 2,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 32,
		.valid_bits_per_sample = 16,
		.speaker_mask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT,
		.settings_file = "max98373-render-2ch-48khz-16b.bin",
	},
};

static struct nhlt_feedback_config render_config = {
	.tdm_config = {
		.virtual_slot = 0x0,
		.config_type = NHLT_TDM_RENDER_FEEDBACK,
	},
	.feedback_virtual_slot = 2,
	.feedback_channels = 4,
	.feedback_valid_bits_per_sample = 16,
};

static struct nhlt_feedback_config capture_config = {
	.tdm_config = {
		.virtual_slot = 0x2,
		.config_type = NHLT_TDM_RENDER_FEEDBACK,
	},
	.feedback_virtual_slot = 0,
	.feedback_channels = 2,
	.feedback_valid_bits_per_sample = 16,
};

static const struct nhlt_endp_descriptor max98373_descriptors[] = {
	{
		.link = NHLT_LINK_SSP,
		.device = NHLT_SSP_DEV_I2S,
		.direction = NHLT_DIR_RENDER,
		.vid = NHLT_VID,
		.did = NHLT_DID_SSP,
		.cfg = &render_config,
		.cfg_size = sizeof(render_config),
		.formats = max98373_render_formats,
		.num_formats = ARRAY_SIZE(max98373_render_formats),
	},
	{
		.link = NHLT_LINK_SSP,
		.device = NHLT_SSP_DEV_I2S,
		.direction = NHLT_DIR_CAPTURE,
		.vid = NHLT_VID,
		.did = NHLT_DID_SSP,
		.cfg = &capture_config,
		.cfg_size = sizeof(capture_config),
		.formats = max98373_capture_formats,
		.num_formats = ARRAY_SIZE(max98373_capture_formats),
	},
};

int nhlt_soc_add_max98373(struct nhlt *nhlt, int hwlink, int render_slot,
				int feedback_slot)
{
	render_config.tdm_config.virtual_slot = render_slot;
	render_config.feedback_virtual_slot = feedback_slot;
	capture_config.tdm_config.virtual_slot = feedback_slot;
	capture_config.feedback_virtual_slot = render_slot;

	/* Virtual bus id of SSP links are the hardware port ids proper. */
	return nhlt_add_ssp_endpoints(nhlt, hwlink, max98373_descriptors,
				      ARRAY_SIZE(max98373_descriptors));
}
