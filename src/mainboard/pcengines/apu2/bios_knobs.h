/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 PC Engines GmbH
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

#ifndef _BIOS_KNOBS_H
#define _BIOS_KNOBS_H

bool check_console(void);
bool check_uartc(void);
bool check_uartd(void);
bool check_ehci0(void);
bool check_mpcie2_clk(void);
int find_knob_index(const char *s, const char *pattern);
size_t get_bootorder_cbfs_offset(const char *name, uint32_t type);


#endif
