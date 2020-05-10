/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_NYAN_SDRAM_CONFIG_H__
#define __MAINBOARD_GOOGLE_NYAN_SDRAM_CONFIG_H__

#include <soc/sdram_param.h>

/* Loads SDRAM configurations for current system. */
const struct sdram_params *get_sdram_config(void);

#endif  /* __MAINBOARD_GOOGLE_NYAN_SDRAM_CONFIG_H__ */
