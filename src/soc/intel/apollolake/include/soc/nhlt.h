/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_APOLLOLAKE_NHLT_H_
#define _SOC_APOLLOLAKE_NHLT_H_

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
int nhlt_soc_add_rt5682(struct nhlt *nhlt, int hwlink);

#endif
