/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stddef.h>
#include <soc/romstage.h>
#include <drivers/intel/fsp1_0/fsp_util.h>

/**
 * /brief mainboard call for setup that needs to be done before fsp init
 *
 */
void early_mainboard_romstage_entry(void)
{

}

/**
 * /brief mainboard call for setup that needs to be done after fsp init
 *
 */
void late_mainboard_romstage_entry(void)
{

}

/**
 * /brief customize fsp parameters here if needed
 */
void romstage_fsp_rt_buffer_callback(FSP_INIT_RT_BUFFER *FspRtBuffer)
{

}
