/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_CANNONLAKE_NHLT_H_
#define _SOC_CANNONLAKE_NHLT_H_

#include <nhlt.h>

#define NHLT_VID 0x8086
#define NHLT_DID_DMIC 0xae20
#define NHLT_DID_BT 0xae30
#define NHLT_DID_SSP 0xae34

/* The following link values should be used for the hwlink parameters below. */
enum {
	AUDIO_LINK_SSP0,
	AUDIO_LINK_SSP1,
	AUDIO_LINK_SSP2,
	AUDIO_LINK_SSP3,
	AUDIO_LINK_SSP4,
	AUDIO_LINK_SSP5,
	AUDIO_LINK_DMIC,
};

/* Returns < 0 on error, 0 on success. */
int nhlt_soc_add_dmic_array(struct nhlt *nhlt, int num_channels);
int nhlt_soc_add_max98357(struct nhlt *nhlt, int hwlink);
int nhlt_soc_add_da7219(struct nhlt *nhlt, int hwlink);
int nhlt_soc_add_max98373(struct nhlt *nhlt, int hwlink);

#endif
