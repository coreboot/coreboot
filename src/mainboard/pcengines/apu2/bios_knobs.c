/*
 * This file is part of the coreboot project.

 *
 * Copyright (C) 2017 3mdeb
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include "bios_knobs.h"
#include <option.h>

#define opt_function(NAME, PAR_NAME, DEFAULT) \
    bool NAME(void) \
    { \
        u8 val; \
        if( get_option (&val, PAR_NAME) == CB_SUCCESS ) \
            return (val == '1')?true:false; \
        else { \
		    printk(BIOS_EMERG, "Missing or invalid " PAR_NAME \
                  " knob, default: " #DEFAULT ".\n"); \
            return DEFAULT; \
        }\
    }

opt_function (check_console, "scon", true)
opt_function (check_uartc, "uartc", false)
opt_function (check_uartd, "uartd", false)
opt_function (check_ehci0, "ehcien", true)
opt_function (check_mpcie2_clk, "mpcie2_clk", false)
