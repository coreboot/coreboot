/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _COMMON_HDA_VERB_H_
#define _COMMON_HDA_VERB_H_

#include <stdint.h>

int hda_codec_detect(u8 *base);
int hda_codec_write(u8 *base, u32 size, const u32 *data);
int hda_codec_init(u8 *base, int addr, int verb_size, const u32 *verb_data);

#endif /* _COMMON_HDA_VERB_H_ */
