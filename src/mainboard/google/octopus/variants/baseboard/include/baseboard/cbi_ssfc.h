/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OCTOPUS_CBI_SSFC__H_
#define _OCTOPUS_CBI_SSFC__H_

#include <inttypes.h>

/****************************************************************************
 * Octopus CBI Second Source Factory Cache
 *
 * SSFC was introduced after variants were MPed already so we can expect there
 * can be devices in the field without SSFC field in the CBI. For devices
 * without SSFC field in the CBI, the value of SSFC will be 0 set by get_ssfc()
 * in the cbi_ssfc.c.
 *
 * On the other hand, taking audio codec as the example there are two sources -
 * DA7219 and RT5682 used in the MPed devices before introducing SSFC. As a
 * result, the value 0 of each bits group for a specific component is defined as
 * DEFAULT and different variants should transform this DEFAULT to one of
 * sources they used as the first sources. In the example here, either DA7219 or
 * RT5682 should be transformed.
 */

/*
 * Audio Codec (Bits 9-11)
 *
 */
enum ssfc_audio_codec {
	SSFC_AUDIO_CODEC_DEFAULT,
	SSFC_AUDIO_CODEC_DA7219,
	SSFC_AUDIO_CODEC_RT5682,
};
#define SSFC_AUDIO_CODEC_OFFSET		9
#define SSFC_AUDIO_CODEC_MASK		0x7

enum ssfc_audio_codec ssfc_get_audio_codec(void);

#endif /* _OCTOPUS_CBI_SSFC__H_ */
