/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/helpers.h>
#include <soc/nhlt.h>

static const struct nhlt_format_config rt5514_4ch_formats[] = {
	/* 48 KHz 16-bits per sample. */
	{
		.num_channels = 4,
		.sample_freq_khz = 48,
		.container_bits_per_sample = 16,
		.valid_bits_per_sample = 16,
		.speaker_mask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT |
				SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT,
		.settings_file = "rt5514-capture-4ch-48khz-16b.bin",
	},
};

static struct nhlt_dmic_array_config rt5514_4ch_mic_config = {
	.tdm_config = {
		.virtual_slot = 0x1,
		.config_type = NHLT_TDM_MIC_ARRAY,
	},
	.array_type = NHLT_MIC_ARRAY_4CH_L_SHAPED,
};

static struct nhlt_endp_descriptor rt5514_4ch_descriptors[] = {
	{
		.link = NHLT_LINK_SSP,
		.device = NHLT_SSP_DEV_I2S,
		.direction = NHLT_DIR_CAPTURE,
		.vid = NHLT_VID,
		.did = NHLT_DID_SSP,
		.cfg = &rt5514_4ch_mic_config,
		.cfg_size = sizeof(rt5514_4ch_mic_config),
		.formats = rt5514_4ch_formats,
		.num_formats = ARRAY_SIZE(rt5514_4ch_formats),
	},
};

int nhlt_soc_add_rt5514(struct nhlt *nhlt, int hwlink, int num_channels, int virtual_slot)
{
	rt5514_4ch_mic_config.tdm_config.virtual_slot = virtual_slot;

	switch (num_channels) {
	case 4:
		return nhlt_add_ssp_endpoints(nhlt, hwlink,
			      rt5514_4ch_descriptors,
			      ARRAY_SIZE(rt5514_4ch_descriptors));
	default:
		return -1;
	}
}
