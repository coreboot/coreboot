/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
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

#ifndef SOUTHBRIDGE_VIA_VT8237R_CHIP_H
#define SOUTHBRIDGE_VIA_VT8237R_CHIP_H

#include <stdint.h>

struct southbridge_via_vt8237r_config {
	/**
	 * Function disable. 1 = disabled.
	 * 7 Dev 17 fn 6 MC97
	 * 6 Dev 17 fn 5 AC97
	 * 5 Dev 16 fn 1 USB 1.1 UHCI Ports 2-3
	 * 4 Dev 16 fn 0 USB 1.1 UHCI Ports 0-1
	 * 3 Dev 15 fn 0 Serial ATA
	 * 2 Dev 16 fn 2 USB 1.1 UHCI Ports 4-5
	 * 1 Dev 16 fn 4 USB 2.0 EHCI
	 * 0 Dev 16 fn 3 USB 1.1 UHCI Ports 6-7
	 */
	u16 fn_ctrl_lo;

	/**
	 * 7 USB Device Mode 1=dis
	 * 6 Reserved
	 * 5 Internal LAN Controller Clock Gating 1=gated
	 * 4 Internal LAN Controller 1=di
	 * 3 Internal RTC 1=en
	 * 2 Internal PS2 Mouse 1=en
	 * 1 Internal KBC Configuration 0=dis ports 0x2e/0x2f off 0xe0-0xef
	 * 0 Internal Keyboard Controller 1=en
	 */
	u16 fn_ctrl_hi;

	u8 ide0_enable;
	u8 ide1_enable;

	/* 1 = 80-pin cable, 0 = 40-pin cable */
	u8 ide0_80pin_cable;
	u8 ide1_80pin_cable;

	u8 usb2_termination_set;
	u8 usb2_termination_a;
	u8 usb2_termination_b;
	u8 usb2_termination_c;
	u8 usb2_termination_d;
	u8 usb2_termination_e;
	u8 usb2_termination_f;
	u8 usb2_termination_g;
	u8 usb2_termination_h;

	u8 usb2_dpll_set;
	u8 usb2_dpll_delay;

	u8 int_efgh_as_gpio;
	u8 enable_gpo3;
	u8 disable_gpo26_gpo27;
	u8 enable_aol_2_smb_slave;
	u8 enable_gpo5;
	u8 gpio15_12_dir_output;
};

#endif /* SOUTHBRIDGE_VIA_VT8237R_CHIP_H */
