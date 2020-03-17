/*
 * This file is part of the coreboot project.
 *
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

#ifndef _DENVERTON_NS_FIAMUX_H
#define _DENVERTON_NS_FIAMUX_H

#include <fsp/util.h>

int get_fiamux_hsio_info(uint16_t num_of_lanes, size_t num_of_entry,
				BL_HSIO_INFORMATION **config);

BL_FIA_MUX_CONFIG_HOB *get_fiamux_hob_data(void);
void print_fiamux_config_hob(BL_FIA_MUX_CONFIG_HOB *fiamux_hob_data);

size_t mainboard_get_hsio_config(BL_HSIO_INFORMATION **p_hsio_config);

#endif // _DENVERTON_NS_FIAMUX_H
