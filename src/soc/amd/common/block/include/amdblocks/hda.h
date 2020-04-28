/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __AMDBLOCKS_HDA_H__
#define __AMDBLOCKS_HDA_H__

#include <device/device.h>

/* SoC callback to add any quirks to HDA device node in SSDT. */
void hda_soc_ssdt_quirks(const struct device *dev);

#endif /* __AMDBLOCKS_HDA_H__ */
