/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Facebook, Inc.
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

#ifndef _SOC_MEMORY_H_
#define _SOC_MEMORY_H_

/* EDS vol 2, 9.2.24 */
#define REG_MC_BIOS_REQ			0x98
#define   REG_MC_BIOS_REQ_FREQ_MSK	((1u << 6) - 1)
#define   REG_MC_MULTIPLIER		133.33f

#define IMC_MAX_CHANNELS		2

#define SPD_SLAVE_ADDR(chan, slot) (2 * chan + slot)

void save_dimm_info(void);

/* Determine if memory configuration has been locked by TXT */
bool memory_config_is_locked(void);


#endif
