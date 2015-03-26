/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef __MAINBOARD_GOOGLE_NYAN_BLAZE_SDRAM_CONFIG_H__
#define __MAINBOARD_GOOGLE_NYAN_BLAZE_SDRAM_CONFIG_H__

#include <soc/sdram_param.h>

/* Loads SDRAM configurations for current system. */
const struct sdram_params *get_sdram_config(void);

#endif  /* __MAINBOARD_GOOGLE_NYAN_BLAZE_SDRAM_CONFIG_H__ */
