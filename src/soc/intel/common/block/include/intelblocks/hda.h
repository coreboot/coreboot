/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_HDA_H
#define SOC_INTEL_COMMON_BLOCK_HDA_H

/* Mainboard overrides. */

/* Mainboard hooks to convey if HD-Audio codec is supported */
bool mainboard_is_hda_codec_enabled(void);

#endif /* SOC_INTEL_COMMON_BLOCK_HDA_H */
