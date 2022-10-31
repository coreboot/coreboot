/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/bsd/helpers.h>
#include <nhlt.h>
#include <soc/nhlt.h>

static const struct nhlt_tdm_config tdm_config = {
	.virtual_slot = 0,
	.config_type = NHLT_TDM_BASIC,
};

static const struct nhlt_format_config da7219_formats[] = {
	/* 48 KHz 24-bits per sample. */
	{
		.num_channels = 2,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 32,
		.valid_bits_per_sample = 24,
		.settings_file = "dialog-2ch-48khz-24b.bin",
	},
};

static const struct nhlt_endp_descriptor da7219_descriptors[] = {
	/* Render Endpoint */
	{
		.link = NHLT_LINK_SSP,
		.device = NHLT_SSP_DEV_I2S,
		.direction = NHLT_DIR_RENDER,
		.vid = NHLT_VID,
		.did = NHLT_DID_SSP,
		.cfg = &tdm_config,
		.cfg_size = sizeof(tdm_config),
		.formats = da7219_formats,
		.num_formats = ARRAY_SIZE(da7219_formats),
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
		.formats = da7219_formats,
		.num_formats = ARRAY_SIZE(da7219_formats),
	},
};

int nhlt_soc_add_da7219(struct nhlt *nhlt, int hwlink)
{
	/* Virtual bus id of SSP links are the hardware port ids proper. */
	return nhlt_add_ssp_endpoints(nhlt, hwlink, da7219_descriptors,
					ARRAY_SIZE(da7219_descriptors));
}
