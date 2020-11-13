/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_HDA_H
#define AMD_BLOCK_HDA_H

#include <device/device.h>

/* SoC callback to add any quirks to HDA device node in SSDT. */
void hda_soc_ssdt_quirks(const struct device *dev);

#endif /* AMD_BLOCK_HDA_H */
