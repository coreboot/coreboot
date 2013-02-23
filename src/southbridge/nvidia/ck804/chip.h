/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
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

#ifndef SOUTHBRIDGE_NVIDIA_CK804_CHIP_H
#define SOUTHBRIDGE_NVIDIA_CK804_CHIP_H

struct southbridge_nvidia_ck804_config {
	unsigned int usb1_hc_reset : 1;
	unsigned int ide0_enable : 1;
	unsigned int ide1_enable : 1;
	unsigned int sata0_enable : 1;
	unsigned int sata1_enable : 1;
	unsigned int mac_eeprom_smbus;
	unsigned int mac_eeprom_addr;
};

#endif
