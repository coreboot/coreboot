/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __AMDBLOCKS_CHIP_H__
#define __AMDBLOCKS_CHIP_H__

struct soc_amd_common_config {
};

/*
 * SoC callback that returns pointer to soc_amd_common_config structure embedded within the chip
 * soc config.
 */
const struct soc_amd_common_config *soc_get_common_config(void);

#endif
