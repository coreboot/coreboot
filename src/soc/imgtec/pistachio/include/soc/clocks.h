/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __SOC_IMGTEC_PISTACHIO_CLOCKS_H__
#define __SOC_IMGTEC_PISTACHIO_CLOCKS_H__

#include <stdint.h>

/* Functions for PLL setting */
int sys_pll_setup(u8 divider1, u8 divider2);
int mips_pll_setup(u8 divider1, u8 divider2, u8 predivider, u32 feedback);

/* Peripheral divider setting */
void system_clk_setup(u8 divider);
void mips_clk_setup(u8 divider1, u8 divider2);
void uart1_clk_setup(u8 divider1, u16 divider2);
void i2c_clk_setup(u8 divider1, u16 divider2, u8 interface);
int usb_clk_setup(u8 divider, u8 refclksel, u8 fsel);
void rom_clk_setup(u8 divider);
void eth_clk_setup(u8 mux, u8 divider);
enum {
	CLOCKS_OK = 0,
	PLL_TIMEOUT = -1,
	USB_TIMEOUT = -2,
	USB_VBUS_FAULT = -3
};

#endif
