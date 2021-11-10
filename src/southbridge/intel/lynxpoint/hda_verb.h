/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef LYNXPOINT_HDA_VERB_H
#define LYNXPOINT_HDA_VERB_H

#include <stdint.h>

int hda_codec_detect(u8 *base);
int hda_codec_init(u8 *base, int addr, int verb_size, const u32 *verb_data);

#endif
