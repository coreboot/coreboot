/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
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

#ifndef EC_LENOVO_H8EC_CHIP_H
#define EC_LENOVO_H8EC_CHIP_H

struct ec_lenovo_h8_config {

	u8 config0;
	u8 config1;
	u8 config2;
	u8 config3;

	u8 beepmask0;
	u8 beepmask1;

	u8 event0_enable;
	u8 event1_enable;
	u8 event2_enable;
	u8 event3_enable;
	u8 event4_enable;
	u8 event5_enable;
	u8 event6_enable;
	u8 event7_enable;
	u8 event8_enable;
	u8 event9_enable;
	u8 eventa_enable;
	u8 eventb_enable;
	u8 eventc_enable;
	u8 eventd_enable;
	u8 evente_enable;
	u8 eventf_enable;

	u8 trackpoint_enable;
	u8 wlan_enable;
	u8 wwan_enable;
};
#endif
